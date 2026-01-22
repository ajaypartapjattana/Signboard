#include "device_builder.h"

#include "Signboard/RHI/core/device.h"
#include "Signboard/RHI/core/instance_vk.h"
#include "Signboard/RHI/core/surface_vk.h"

#include <stdexcept>
#include <unordered_map>

namespace rhi::procedure {

	device_builder::device_builder(const rhi::core::instance& instance) {
		VkInstance vkInstance = rhi::core::instance_vkAccess::get(instance);

		uint32_t count = 0;
		vkEnumeratePhysicalDevices(vkInstance, &count, nullptr);

		if (count == 0)
			throw std::runtime_error("No physical device with vulkan support found!");

		std::vector<VkPhysicalDevice> devices(count);
		vkEnumeratePhysicalDevices(vkInstance, &count, devices.data());

		m_candidates.clear();
		m_candidates.reserve(count);

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
			

			m_candidates.push_back(c);
		}

	}
	
	device_builder& device_builder::require_graphicsQueue() {
		physical_device_selector selector;
		selector.require_Queue(*this, VK_QUEUE_GRAPHICS_BIT);
		return *this;
	}

	device_builder& device_builder::require_computeQueue() {
		physical_device_selector selector;
		selector.require_Queue(*this, VK_QUEUE_COMPUTE_BIT);
		return *this;
	}

	device_builder& device_builder::require_transferQueue() {
		physical_device_selector selector;
		selector.require_Queue(*this, VK_QUEUE_TRANSFER_BIT);
		return *this;
	}

	device_builder& device_builder::require_presentQueue(const rhi::core::surface& s) {
		VkSurfaceKHR surface =  rhi::core::surface_vkAccess::get(s);
		m_surface = surface;

		physical_device_selector selector;
		selector.require_presentSupport(*this);
		selector.require_extension(*this, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		return *this;
	}

	device_builder& device_builder::enable_anisotropy() {
		physical_device_selector selector;
		selector.require_feature(*this, &VkPhysicalDeviceFeatures::samplerAnisotropy);
		return *this;
	}

	rhi::core::device device_builder::build() {
		const std::vector<phys_candidate>& candidates = m_candidates;

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
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = m_requiredExtensions.data();
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
		for (const phys_candidate::assigned_queue& aq : suited_physical->assigned_queueFamilies) {
			VkQueue vkQueue = queues.at(aq.family);
			device.m_queues.push_back({ vkQueue, aq.family, aq.caps, aq.can_present });
		}

		return device;
	}
	
	class physical_device_selector {
	public:
		void require_Queue(device_builder& b, VkQueueFlags required) {
			for (device_builder::phys_candidate& c : b.m_candidates) {
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

		void require_presentSupport(device_builder& b) {
			for (device_builder::phys_candidate& c : b.m_candidates) {
				if (!c.suitable)
					continue;

				bool found = false;
				for (device_builder::phys_candidate::assigned_queue& q : c.assigned_queueFamilies) {
					VkBool32 supported = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(c.phys, q.family, b.m_surface, &supported);

					if (supported) {
						q.can_present = true;
						found = true;
						break;
					}
				}

				if (!found) {
					for (uint32_t i = 0; i < c.families.size(); ++i) {
						VkBool32 supported = false;
						vkGetPhysicalDeviceSurfaceSupportKHR(c.phys, i, b.m_surface, &supported);

						if (supported) {
							VkQueueFlags caps = c.families[i].queueFlags;
							c.assigned_queueFamilies.push_back({i, caps, true});
							found = true;
							break;
						}
					}
				}

				if (!found)
					c.suitable = false;
			}
		}

		void require_extension(device_builder& b, const char* name) {
			bool supported = false;
			for (device_builder::phys_candidate& c : b.m_candidates) {
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

			b.m_requiredExtensions.push_back(name);
		}

		void require_feature(device_builder& b, VkBool32 VkPhysicalDeviceFeatures::* member) {
			for (device_builder::phys_candidate& c : b.m_candidates) {
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