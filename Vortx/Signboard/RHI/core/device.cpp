#include "device.h"

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <unordered_map>
#include <cstring>

#include "instance_vk.h"
#include "surface_vk.h"

namespace rhi::core {

	device::device(const device_CI& createInfo, const instance& instance)
		: m_device(VK_NULL_HANDLE), m_physical(VK_NULL_HANDLE)
	{
		VkInstance vkInstance = rhi::core::instance_vkAccess::get(instance);
		build(createInfo, vkInstance);
	}

	device::device(device&& other) noexcept {
		m_device = other.m_device;
		m_physical = other.m_physical;
		m_queues = other.m_queues;
		m_enabledfeatures = other.m_enabledfeatures;
		m_properties = other.m_properties;

		other.m_device = VK_NULL_HANDLE;
		other.m_physical = VK_NULL_HANDLE;
		other.m_queues.clear();
	}

	device& device::operator=(device&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_device)
			vkDestroyDevice(m_device, nullptr);

		m_device = other.m_device;
		m_physical = other.m_physical;
		m_queues = other.m_queues;
		m_enabledfeatures = other.m_enabledfeatures;
		m_properties = other.m_properties;

		other.m_device = VK_NULL_HANDLE;
		other.m_physical = VK_NULL_HANDLE;
		other.m_queues.clear();

		return *this;
	}

	device::~device() noexcept {
		if (m_device != VK_NULL_HANDLE)
			vkDestroyDevice(m_device, nullptr);
	}

	const VkDevice* device::native_device() const noexcept {
		return &m_device;
	}

	void device::build(const device_CI& ci, const VkInstance instance) {

		struct phys_candidate {
			VkPhysicalDevice phys = VK_NULL_HANDLE;
			bool suitable = true;

			std::vector<VkQueueFamilyProperties> families;
			std::vector<VkExtensionProperties> extensions;
			VkPhysicalDeviceFeatures features;

			struct assigned_queue {
				uint32_t family;
				uint32_t index = 0;
				VkQueueFlags caps;
				bool can_present = false;
			};

			std::vector<assigned_queue> assigned_queueFamilies;

			VkPhysicalDeviceProperties properties{};
			VkPhysicalDeviceFeatures enabledFeatures{};
		};

		std::vector<phys_candidate> candidates;
		std::vector<const char*> m_reqExt = ci.requiredExtensions;

		uint32_t count = 0;
		vkEnumeratePhysicalDevices(instance, &count, nullptr);

		if (count == 0)
			throw std::runtime_error("ABORT: vulkan_not_supported!");

		std::vector<VkPhysicalDevice> devices(count);
		vkEnumeratePhysicalDevices(instance, &count, devices.data());

		candidates.reserve(count);

		for (const VkPhysicalDevice phys : devices) {
			phys_candidate c{};
			c.phys = phys;
			c.assigned_queueFamilies.reserve(ci.requiredQueues.size());

			uint32_t qCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(phys, &qCount, nullptr);

			c.families.resize(qCount);
			vkGetPhysicalDeviceQueueFamilyProperties(phys, &qCount, c.families.data());

			uint32_t extCount = 0;
			vkEnumerateDeviceExtensionProperties(phys, nullptr, &extCount, nullptr);

			c.extensions.resize(extCount);
			vkEnumerateDeviceExtensionProperties(phys, nullptr, &extCount, c.extensions.data());

			vkGetPhysicalDeviceProperties(phys, &c.properties);

			vkGetPhysicalDeviceFeatures(c.phys, &c.features);

			candidates.emplace_back(std::move(c));
		}

		for (const VkQueueFlagBits required : ci.requiredQueues) {
			for (phys_candidate& c : candidates) {
				if (!c.suitable)
					continue;

				VkQueueFlags remaining = required;

				while (remaining) {
					uint32_t bestFamily = UINT32_MAX;
					uint32_t bestIndex = 0;
					VkQueueFlags bestMatched = 0;
					bool bestDedicated = false;
					uint32_t bestExtraCaps = UINT32_MAX;

					const uint32_t familyCount = static_cast<uint32_t>(c.families.size());
					for (uint32_t i = 0; i < familyCount && remaining; ++i) {
						VkQueueFlags supported = c.families[i].queueFlags;
						VkQueueFlags matched = supported & remaining;
						if (!matched)
							continue;

						uint32_t assignedCount = 0;
						for (const auto& q : c.assigned_queueFamilies) {
							if (q.family == i)
								++assignedCount;
						}

						if (assignedCount >= c.families[i].queueCount)
							continue;

						bool dedicated = false;
						if (matched & VK_QUEUE_TRANSFER_BIT)
							dedicated = (supported & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) == 0;
						else if (matched & VK_QUEUE_COMPUTE_BIT)
							dedicated = (supported & VK_QUEUE_GRAPHICS_BIT);

						uint32_t extraCaps = static_cast<uint32_t>(supported & ~matched);

						bool better = false;
						if (bestFamily == UINT32_MAX)
							better = true;
						else if (dedicated != bestDedicated)
							better = dedicated;
						else if (extraCaps < bestExtraCaps)
							better = true;

						if (better) {
							bestFamily = i;
							bestIndex = assignedCount;
							bestMatched = matched;
							bestDedicated = dedicated;
							bestExtraCaps = extraCaps;
						}
					}
					if (bestFamily == UINT32_MAX)
						break;

					c.assigned_queueFamilies.push_back({ bestFamily, bestIndex, bestMatched, false });
					remaining &= ~bestMatched;
				}

				if (remaining != 0)
					c.suitable = false;
			}
		}

		if (ci.present_surface) {
			m_reqExt.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			VkSurfaceKHR m_surface = rhi::core::surface_vkAccess::get(*ci.present_surface);

			for (phys_candidate& c : candidates) {
				if (!c.suitable)
					continue;

				bool found = false;
				for (phys_candidate::assigned_queue& q : c.assigned_queueFamilies) {
					VkBool32 supported = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(c.phys, q.family, m_surface, &supported);

					if (supported) {
						q.can_present = true;
						found = true;
						break;
					}
				}

				if (!found) {
					for (uint32_t i = 0; i < c.families.size(); ++i) {
						VkBool32 supported = false;
						vkGetPhysicalDeviceSurfaceSupportKHR(c.phys, i, m_surface, &supported);

						if (supported) {
							VkQueueFlags caps = c.families[i].queueFlags;
							c.assigned_queueFamilies.push_back({ i, 0, caps, true });
							found = true;
							break;
						}
					}
				}

				if (!found)
					c.suitable = false;
			}
		}

		for (const char* name : m_reqExt) {
			bool supported = false;
			for (phys_candidate& c : candidates) {
				if (!c.suitable)
					continue;

				bool found = false;
				for (const VkExtensionProperties& ext : c.extensions) {
					if (strcmp(ext.extensionName, name) == 0) {
						found = true;
						break;
					}
				}

				if (!found)
					c.suitable = false;
				else
					supported = true;
			}
			if (!supported)
				throw std::runtime_error(std::string("HARDWARE_LIMIT: extension_not_supported!") + name);

		}

		for (VkBool32 VkPhysicalDeviceFeatures::* feature : ci.requiredFeatures) {
			for (phys_candidate& c : candidates) {
				if (!c.suitable)
					continue;

				if ((c.features.*feature))
					c.enabledFeatures.*feature = VK_TRUE;
				else
					c.suitable = false;
			}
		}

		auto suited_physical = std::find_if(candidates.begin(), candidates.end(), [](const phys_candidate& c) {return c.suitable; });

		if (suited_physical == candidates.end())
			throw std::runtime_error("FAILURE: no_satisfactory_physicalDevice!");

		std::unordered_map<uint32_t, uint32_t> familyQueueCount;
		for (const auto& asq : suited_physical->assigned_queueFamilies) {
			uint32_t& stored = familyQueueCount[asq.family];
			stored = std::max(stored, asq.index + 1);

			auto it = familyQueueCount.find(asq.family);
			uint32_t needed = asq.index + 1;
			if (it == familyQueueCount.end())
				familyQueueCount.emplace(asq.family, needed);
			else if (it->second < needed)
				it->second = needed;
		}

		std::vector<std::vector<float>> queuePriorities;
		queuePriorities.reserve(familyQueueCount.size());

		std::vector<VkDeviceQueueCreateInfo> queueInfos;
		queueInfos.reserve(familyQueueCount.size());

		for (const auto& [family, queueCount] : familyQueueCount) {
			queuePriorities.emplace_back(queueCount, 1.0f);

			VkDeviceQueueCreateInfo qi{};
			qi.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			qi.queueFamilyIndex = family;
			qi.queueCount = queueCount;
			qi.pQueuePriorities = queuePriorities.back().data();

			queueInfos.push_back(qi);
		}

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
		createInfo.pQueueCreateInfos = queueInfos.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_reqExt.size());
		createInfo.ppEnabledExtensionNames = m_reqExt.data();
		createInfo.pEnabledFeatures = &suited_physical->enabledFeatures;

		VkResult result = vkCreateDevice(suited_physical->phys, &createInfo, nullptr, &m_device);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: device_creation!");

		m_physical = suited_physical->phys;

		for (const phys_candidate::assigned_queue& aq : suited_physical->assigned_queueFamilies) {
			VkQueue vk_queue = VK_NULL_HANDLE;
			vkGetDeviceQueue(m_device, aq.family, aq.index, &vk_queue);
			m_queues.push_back({ vk_queue, aq.family, aq.index, aq.caps, aq.can_present });
		}

		m_enabledfeatures = suited_physical->enabledFeatures;
		m_properties = suited_physical->properties;

	}

}