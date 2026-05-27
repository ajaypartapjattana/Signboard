#include "device.hh"

namespace rhi {

	VkResult device::create(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo) noexcept {
		VkDevice device;
		VkResult result = vkCreateDevice(physicalDevice, pCreateInfo, nullptr, &device);
		if (result != VK_SUCCESS)
			return result;

		reset();

		if (pCreateInfo->pEnabledFeatures)
			enabledFeatures = std::make_unique<VkPhysicalDeviceFeatures>(*pCreateInfo->pEnabledFeatures);

		{
			uint32_t familyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);

			familyQueueCount.resize(familyCount, 0);
		}

		for (uint32_t i = 0; i < pCreateInfo->queueCreateInfoCount; ++i) {
			const VkDeviceQueueCreateInfo& queueInfo = pCreateInfo->pQueueCreateInfos[i];
			familyQueueCount[queueInfo.queueFamilyIndex] = queueInfo.queueCount;
		}

		m_device = device;

		return VK_SUCCESS;
	}

	void device::reset() noexcept {
		if (m_device)
			vkDestroyDevice(m_device, nullptr);

		m_device = VK_NULL_HANDLE;
		familyQueueCount.clear();
		enabledFeatures.reset();
	}

}