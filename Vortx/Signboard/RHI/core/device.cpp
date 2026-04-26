#include "device.h"

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <cstring>
#include <set>

#include "instance_pAccess.h"
#include "surface_pAccess.h"

namespace rhi {

	creDevice::creDevice(const createInfo& createInfo, const creInstance& instance)
		: 
		r_device(VK_NULL_HANDLE), 
		m_physical(VK_NULL_HANDLE)
	{
		VkInstance vkInstance = rhi::access::instance_pAccess::get(instance);
		build(createInfo, vkInstance);
	}

	creDevice::creDevice(creDevice&& other) noexcept 
		: 
		r_device(other.r_device),
		m_physical(other.m_physical),
		m_queues(std::move(other.m_queues)),
		m_enabledFeatures(std::move(other.m_enabledFeatures))
	{
		other.r_device = VK_NULL_HANDLE;
	}

	creDevice& creDevice::operator=(creDevice&& other) noexcept {
		if (this == &other)
			return *this;

		if (r_device)
			vkDestroyDevice(r_device, nullptr);

		r_device = other.r_device;
		m_physical = other.m_physical;
		m_queues = std::move(other.m_queues);
		m_enabledFeatures = std::move(other.m_enabledFeatures);

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	creDevice::~creDevice() noexcept {
		if (r_device != VK_NULL_HANDLE)
			vkDestroyDevice(r_device, nullptr);
	}

	bool creDevice::active_feature(const VkBool32 VkPhysicalDeviceFeatures::* feature) const noexcept {
		return std::find(m_enabledFeatures.begin(),	m_enabledFeatures.end(), feature) != m_enabledFeatures.end();
	}

	void creDevice::build(const createInfo& ci, const VkInstance instance) {
		struct phys_candidate {
			VkPhysicalDevice phys = VK_NULL_HANDLE;
			bool suitable = true;
			uint32_t type_score = 0;
			uint32_t queue_score = 0;

			standardQueueFamilies assigned_indecies ;

			VkPhysicalDeviceFeatures enabledFeatures;
		};
		std::vector<phys_candidate> candidates;

		std::vector<const char*> a_requiredExtensions = ci.requiredExtensions;
		if(ci.present_surface) a_requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		uint32_t count = 0;
		vkEnumeratePhysicalDevices(instance, &count, nullptr);
		if (count == 0) throw std::runtime_error("ABORT: vulkan_not_supported!");

		std::vector<VkPhysicalDevice> devices{ count };
		vkEnumeratePhysicalDevices(instance, &count, devices.data());

		candidates.reserve(count);
		for (const VkPhysicalDevice phys : devices) {
			phys_candidate c{};
			c.phys = phys;

			uint32_t qCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(phys, &qCount, nullptr);
			std::vector<VkQueueFamilyProperties> families{ qCount };
			vkGetPhysicalDeviceQueueFamilyProperties(phys, &qCount, families.data());
			standardQueueFamilies& idx = c.assigned_indecies;
			for (uint32_t i = 0; i < qCount; ++i) {
				const VkQueueFlags& q = families[i].queueFlags;

				if ((q & VK_QUEUE_GRAPHICS_BIT) && idx.graphics == UINT32_MAX)
					idx.graphics = i;
			}

			for (uint32_t i = 0; i < qCount; ++i) {
				const VkQueueFlags& q = families[i].queueFlags;

				if ((q & VK_QUEUE_COMPUTE_BIT) && !(q & VK_QUEUE_GRAPHICS_BIT)) {
					idx.compute = i;
					break;
				}
			}
			if (idx.compute == UINT32_MAX) idx.compute = idx.graphics;

			for (uint32_t i = 0; i < qCount; ++i) {
				const VkQueueFlags& q = families[i].queueFlags;

				if ((q & VK_QUEUE_TRANSFER_BIT) && !(q & VK_QUEUE_GRAPHICS_BIT) && !(q & VK_QUEUE_COMPUTE_BIT)) {
					idx.transfer = i;
					break;
				}
			}
			if (idx.transfer == UINT32_MAX) idx.transfer = idx.compute;

			if (ci.present_surface) {
				VkSurfaceKHR a_surface = rhi::access::surface_pAccess::extract(*ci.present_surface);
				for (uint32_t i = 0; i < qCount; ++i) {
					VkBool32 supported = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(phys, i, a_surface, &supported);

					if (supported) {
						idx.present = i;
						break;
					}
				}
				if (idx.present == UINT32_MAX)
					c.suitable = false;
			} else {
				idx.present = idx.graphics;
			}

			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(c.phys, &deviceFeatures);
			for (VkBool32 VkPhysicalDeviceFeatures::* feature : ci.requiredFeatures) {
				if (!c.suitable)
					continue;

				if ((deviceFeatures.*feature))
					c.enabledFeatures.*feature = VK_TRUE;
				else
					c.suitable = false;
			}

			uint32_t extCount = 0;
			vkEnumerateDeviceExtensionProperties(phys, nullptr, &extCount, nullptr);
			std::vector<VkExtensionProperties> extensions{ extCount };
			vkEnumerateDeviceExtensionProperties(phys, nullptr, &extCount, extensions.data());
			for (const char* name : a_requiredExtensions) {
				if (!c.suitable)
					continue;

				bool found = false;
				for (const VkExtensionProperties& ext : extensions) {
					if (strcmp(ext.extensionName, name) == 0) {
						found = true;
						break;
					}
				}

				if (!found)
					c.suitable = false;
			}

			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(phys, &properties);
			switch (properties.deviceType) {
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				c.type_score += 1000;
				break;
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				c.type_score += 200;
				break;
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				c.type_score += 100;
				break;
			default:
				break;
			}

			c.queue_score = 0;
			for (const auto& f : families)
			{
				c.queue_score += 1;

				if ((f.queueFlags & VK_QUEUE_TRANSFER_BIT) &&
					!(f.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)))
					c.queue_score += 2;

				if ((f.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
					!(f.queueFlags & VK_QUEUE_GRAPHICS_BIT))
					c.queue_score += 1;
			}

			candidates.emplace_back(std::move(c));
		}


		const phys_candidate* suited_physical = nullptr;
		for (const phys_candidate& c : candidates) {
			if (!c.suitable)
				continue;

			if (!suited_physical || c.type_score > suited_physical->type_score || (c.type_score == suited_physical->type_score && c.queue_score > suited_physical->queue_score)) {
				suited_physical = &c;
			}
		}
		if (!suited_physical)
			throw std::runtime_error("FAILURE: no_satisfactory_physicalDevice!");

		const standardQueueFamilies& idx = suited_physical->assigned_indecies;
		std::set<uint32_t> uniqueFamilies = { idx.graphics, idx.compute, idx.transfer, idx.present };

		std::vector<float> priority = { 1.0f };

		std::vector<VkDeviceQueueCreateInfo> queueInfos;
		queueInfos.reserve(static_cast<uint32_t>(uniqueFamilies.size()));

		for (uint32_t family : uniqueFamilies) {
			VkDeviceQueueCreateInfo qi{};
			qi.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			qi.queueFamilyIndex = family;
			qi.queueCount = 1;
			qi.pQueuePriorities = priority.data();

			queueInfos.push_back(qi);
		}

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
		createInfo.pQueueCreateInfos = queueInfos.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(a_requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = a_requiredExtensions.data();
		createInfo.pEnabledFeatures = &suited_physical->enabledFeatures;

		VkResult result = vkCreateDevice(suited_physical->phys, &createInfo, nullptr, &r_device);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: device_creation!");

		m_physical = suited_physical->phys;

		vkGetDeviceQueue(r_device, idx.graphics, 0, &m_queues.graphics);
		vkGetDeviceQueue(r_device, idx.compute, 0, &m_queues.compute);
		vkGetDeviceQueue(r_device, idx.transfer, 0, &m_queues.transfer);
		vkGetDeviceQueue(r_device, idx.present, 0, &m_queues.present);

		m_families.graphics = idx.graphics;
		m_families.compute = idx.compute;
		m_families.transfer = idx.transfer;
		m_families.present = idx.present;

		m_enabledFeatures = ci.requiredFeatures;

	}

}