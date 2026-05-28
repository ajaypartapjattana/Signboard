#pragma once

#include "Signboard/Core/core.h"
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
		VkSurfaceTransformFlagBitsKHR transform;

		rhi::swapchain m_swapchain;


	public:
		presentation() = default;
		presentation(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) noexcept {

		}
		presentation(const presentation&) = delete;
		presentation(presentation&& other) noexcept 
			:
			m_swapchain(std::move(other.m_swapchain))
		{

		}

		presentation& operator=(const presentation&) = delete;
		presentation& operator=(presentation&& other) noexcept {

		}

		void targetPresentationDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) noexcept;

		void createSwapchain(VkDevice device) noexcept;
		void reset() noexcept;

		void swapchainImageCount(uint32_t count) noexcept;
		void constructSwapchain(const rhi::pcdWatchdog& watchdog);

		void validateSwapchain() noexcept;

		uint32_t availableImageCount() const noexcept;

	};

}

