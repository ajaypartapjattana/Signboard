#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <utility>
#include <memory>

namespace rhi {

	class device {
	private:
		std::unique_ptr<VkPhysicalDeviceFeatures> enabledFeatures;
		std::vector<uint32_t> familyQueueCount;

		VkDevice m_device = VK_NULL_HANDLE;

	public:
		device() = default;
		device(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo) {
			VkResult result = create(physicalDevice, pCreateInfo);
			if (result != VK_SUCCESS)
				throw std::runtime_error("FAILURE: vulkan_device_creation!");
		}
		device(const device&) = delete;
		device(device&& other) noexcept 
			:
			enabledFeatures(std::move(other.enabledFeatures)),
			familyQueueCount(std::move(other.familyQueueCount)),
			m_device(std::exchange(other.m_device, VK_NULL_HANDLE))
		{

		}
		
		device& operator=(const device&) = delete;
		device& operator=(device&& other) noexcept {
			if (this == &other)
				return *this;

			reset();

			enabledFeatures = std::move(other.enabledFeatures);
			familyQueueCount = std::move(other.familyQueueCount);
			m_device = std::exchange(other.m_device, VK_NULL_HANDLE);

			return *this;
		}

		~device() noexcept {
			reset();
		}

		operator VkDevice() const noexcept {
			return m_device;
		}

		bool active_feature(const VkBool32 VkPhysicalDeviceFeatures::* feature) const noexcept {
			return enabledFeatures && ((*enabledFeatures).*feature == VK_TRUE);
		}

		VkResult create(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	};

}