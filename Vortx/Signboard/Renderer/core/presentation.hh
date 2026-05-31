#pragma once

#include "Signboard/RHI/core/swapchain.hh"

namespace rndr {

	class context;

	struct _pAccess;

	class presentation {
	private:
		VkPhysicalDevice physicalDeviceCandidate = VK_NULL_HANDLE;
		VkSurfaceKHR surfaceCandidate = VK_NULL_HANDLE;

		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;

		uint32_t bufferedImageCount;
		VkExtent2D extent;
		VkSharingMode imageSharingMode;
		VkSurfaceTransformFlagBitsKHR transform;

		rhi::swapchain m_swapchain;


	public:
		presentation() = default;
		presentation(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) noexcept {
			presentationConfiguration(physicalDevice, surface);
		}
		presentation(const presentation&) = delete;
		presentation(presentation&& other) noexcept 
			:
			physicalDeviceCandidate(other.physicalDeviceCandidate),
			surfaceCandidate(other.surfaceCandidate),
			surfaceFormat(other.surfaceFormat),
			presentMode(other.presentMode),
			bufferedImageCount(other.bufferedImageCount),
			extent(other.extent),
			imageSharingMode(other.imageSharingMode),
			transform(other.transform),
			m_swapchain(std::move(other.m_swapchain))
		{
			
		}

		presentation& operator=(const presentation&) = delete;
		presentation& operator=(presentation&& other) noexcept {
			if (this == &other)
				return *this;

			physicalDeviceCandidate = other.physicalDeviceCandidate;
			surfaceCandidate = other.surfaceCandidate;
			surfaceFormat = other.surfaceFormat;
			presentMode = other.presentMode;
			bufferedImageCount = other.bufferedImageCount;
			extent = other.extent;
			imageSharingMode = other.imageSharingMode;
			transform = other.transform;
			m_swapchain = std::move(other.m_swapchain);

			return *this;
		}

		void createSwapchain(VkDevice device, uint32_t graphicsFamilyIndex, uint32_t presentFamilyIndex);

		void presentationConfiguration(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) noexcept;
		void reset() noexcept;

	};

}

