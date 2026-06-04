#pragma once

#include "Signboard/RHI/core/swapchain.hh"

struct PresentationInfo {
	VkSwapchainKHR swapchain;
	uint32_t swapchainImageCount;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainImageExtent;
};

namespace rndr {

	class presentation {
	private:
		VkPhysicalDevice physicalDeviceCandidate = VK_NULL_HANDLE;
		VkSurfaceKHR surfaceCandidate = VK_NULL_HANDLE;

		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;

		rhi::swapchain swapchain;

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
			swapchain(std::move(other.swapchain))
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
			swapchain = std::move(other.swapchain);

			return *this;
		}

		void presentationConfiguration(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) noexcept;
		void createSwapchain(VkDevice device, uint32_t graphicsFamilyIndex, uint32_t presentFamilyIndex, PresentationInfo* pInfo);

		void reset() noexcept;

	};

}

