#include "device_builder.h"

#include "Signboard/RHI/core/instance_vk.h"
#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/core/surface_vk.h"

#include <stdexcept>
#include <unordered_map>

namespace rhi::procedure {

	struct phys_candidate {
		VkPhysicalDevice phys = VK_NULL_HANDLE;
		bool suitable = true;

		std::vector<VkQueueFamilyProperties> families;
		std::vector<VkExtensionProperties> extensions;
		VkPhysicalDeviceFeatures features;

		struct assigned_queue {
			uint32_t family;
			VkQueueFlags caps;
		};

		std::vector<assigned_queue> assigned_queueFamilies;
		VkPhysicalDeviceFeatures enabledFeatures{};

	};

	struct device_builder::Impl{
		std::vector<phys_candidate> g_candidates;

		std::vector<const char*> m_requiredExtensions;
		VkSurfaceKHR surface;
	};

	device_builder::device_builder(const rhi::core::instance& instance) 
		: m_instance(instance), impl(new Impl)
	{
		VkInstance vkInstance = rhi::core::instance_vkAccess::get(instance);

		uint32_t count = 0;
		vkEnumeratePhysicalDevices(vkInstance, &count, nullptr);

		if (count == 0)
			throw std::runtime_error("No physical device with vulkan support found!");

		std::vector<VkPhysicalDevice> devices(count);
		vkEnumeratePhysicalDevices(vkInstance, &count, devices.data());

		Impl& build_state = *impl;

		build_state.g_candidates.clear();
		build_state.g_candidates.reserve(count);

		for (VkPhysicalDevice phys : devices) {
			phys_candidate c{};
			c.phys = phys;

			uint32_t qCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(phys, &qCount, nullptr);

			c.families.resize(qCount);
			vkGetPhysicalDeviceQueueFamilyProperties(phys, &qCount, c.families.data());

			uint32_t extCount = 0;
			vkEnumerateDeviceExtensionProperties(phys, nullptr, &extCount, nullptr);

			c.extensions.resize(extCount);
			vkEnumerateDeviceExtensionProperties(phys, nullptr, &extCount, c.extensions.data());

			vkGetPhysicalDeviceFeatures(c.phys, &c.features);
			

			build_state.g_candidates.push_back(c);
		}

	}
	
	device_builder& device_builder::require_graphicsQueue() {
		physical_device_selector selector;
		selector.requireQueue(*impl, VK_QUEUE_GRAPHICS_BIT);
		return *this;
	}

	device_builder& device_builder::require_computeQueue() {
		physical_device_selector selector;
		selector.requireQueue(*impl, VK_QUEUE_COMPUTE_BIT);
		return *this;
	}

	device_builder& device_builder::require_transferQueue() {
		physical_device_selector selector;
		selector.requireQueue(*impl, VK_QUEUE_TRANSFER_BIT);
		return *this;
	}

	device_builder& device_builder::require_presentQueue(const rhi::core::surface& s) {
		VkSurfaceKHR surface =  rhi::core::surface_vkAccess::get(s);
		impl->surface = surface;
		//remaining queue present capability check in physical device selector!
	}

	device_builder& device_builder::enable_anisotropy() {
		physical_device_selector selector;
		selector.requireFeature(*impl, &VkPhysicalDeviceFeatures::samplerAnisotropy);
		return *this;
	}

	rhi::core::device device_builder::build() {
		const Impl& build_state = *impl;
		const std::vector<phys_candidate>& candidates = build_state.g_candidates;

		auto suited_physical = std::find_if(candidates.begin(), candidates.end(), [](const phys_candidate& c) {return c.suitable; });

		if (suited_physical == candidates.end())
			throw std::runtime_error("No suitable physical device found!");

		std::unordered_map<uint32_t, uint32_t> familyQueueCount;
		std::vector<VkDeviceQueueCreateInfo> queueInfos;

		for (const auto& c : suited_physical->assigned_queueFamilies) {
			if (familyQueueCount.emplace(c.family, 1).second) {
				static const float priority = 1.0f;

				VkDeviceQueueCreateInfo qi{};
				qi.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				qi.queueFamilyIndex = c.family;
				qi.queueCount = 1;
				qi.pQueuePriorities = &priority;

				queueInfos.push_back(qi);
			}
		}

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
		createInfo.pQueueCreateInfos = queueInfos.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(build_state.m_requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = build_state.m_requiredExtensions.data();
		createInfo.pEnabledFeatures = &suited_physical->enabledFeatures;

		VkDevice vkDevice = VK_NULL_HANDLE;
		if (vkCreateDevice(suited_physical->phys, &createInfo, nullptr, &vkDevice) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device!");

		std::unordered_map<uint32_t, VkQueue> queues;
		for (const auto& [family, _] : familyQueueCount) {
			VkQueue q = VK_NULL_HANDLE;
			vkGetDeviceQueue(vkDevice, family, 0, &q);
			queues[family] = q;
		}

		rhi::core::device device;
		device.m_device = vkDevice;
		device.m_physical = suited_physical->phys;

		return device;
	}
	
	class physical_device_selector {
	public:
		void requireQueue(device_builder::Impl& impl, VkQueueFlags required) {
			for (phys_candidate& c : impl.g_candidates) {
				if (!c.suitable)
					continue;

				VkQueueFlags remaining = required;

				for (const auto& q : c.assigned_queueFamilies) {
					VkQueueFlags reused = q.caps & remaining;
					remaining &= ~reused;
				}

				for (uint32_t i = 0; i < c.families.size() && remaining; ++i) {
					VkQueueFlags supported = c.families[i].queueFlags;

					VkQueueFlags matched = supported & remaining;
					if (matched) {
						c.assigned_queueFamilies.push_back({ i, matched });
						remaining &= ~matched;
					}
				}

				if (remaining != 0) {
					c.suitable = false;
				}
			}
		}

		void requireExtension(device_builder::Impl& impl, const char* name) {
			bool supported = false;
			for (phys_candidate& c : impl.g_candidates) {
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
				throw std::runtime_error("The extension is not supportes by any physical device!");

			impl.m_requiredExtensions.push_back(name);
		}

		void requireFeature(device_builder::Impl& impl, VkBool32 VkPhysicalDeviceFeatures::* member) {
			for (phys_candidate& c : impl.g_candidates) {
				if (!c.suitable)
					continue;

				if ((c.features.*member))
					c.enabledFeatures.*member = VK_TRUE;
				else
					c.suitable = false;
			}

		}

	};

}