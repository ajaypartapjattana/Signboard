#include "Signboard/Renderer/Internal/rndr_pAccess.h"

namespace rndr {

	void presentation::presentationConfiguration(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface) noexcept {
		VkSurfaceFormatKHR selectedSurfaceFormat{};
		
		{
			std::array<VkSurfaceFormatKHR, 4> prioritisedSurfaceFormats = {

				VkSurfaceFormatKHR{
					VK_FORMAT_B8G8R8A8_SRGB,
					VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
				},

				VkSurfaceFormatKHR{
					VK_FORMAT_R8G8B8A8_SRGB,
					VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
				},

				VkSurfaceFormatKHR{
					VK_FORMAT_B8G8R8A8_UNORM,
					VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
				},

				VkSurfaceFormatKHR{
					VK_FORMAT_R8G8B8A8_UNORM,
					VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
				}
			};

			uint32_t supportedFormatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatCount, nullptr);

			std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatCount, supportedFormats.data());

			bool found = false;

			uint32_t prioritiesedCount = static_cast<uint32_t>(prioritisedSurfaceFormats.size());
			for (uint32_t i = 0; (i < prioritiesedCount) && (!found); ++i) {
				for (uint32_t j = 0; (j < supportedFormatCount) && (!found); ++j) {
					const VkSurfaceFormatKHR& prioritised = prioritisedSurfaceFormats[i];
					const VkSurfaceFormatKHR& supported = supportedFormats[j];

					if (prioritised.format != supported.format)
						continue;

					if (prioritised.colorSpace != supported.colorSpace)
						continue;

					selectedSurfaceFormat = supported;

					found = true;
				}
			}

			if (!found)
				selectedSurfaceFormat = supportedFormats[0];

		}
		
		VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		{
			std::array<VkPresentModeKHR, 3> prioritisedPresentModes = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR , VK_PRESENT_MODE_IMMEDIATE_KHR };

			uint32_t supportedPresentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &supportedPresentModeCount, nullptr);

			std::vector<VkPresentModeKHR> supportedPresentModes(supportedPresentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &supportedPresentModeCount, supportedPresentModes.data());

			bool found = false;

			uint32_t prioritisedCount = static_cast<uint32_t>(prioritisedPresentModes.size());
			for (uint32_t i = 0; (i < prioritisedCount) && (!found); ++i) {
				for (uint32_t j = 0; (j < supportedPresentModeCount) && (!found); ++j) {
					if (prioritisedPresentModes[i] != supportedPresentModes[j])
						continue;

					selectedPresentMode = supportedPresentModes[j];
					found = true;
				}
			}
		}

		surfaceFormat = selectedSurfaceFormat;
		presentMode = selectedPresentMode;
	}

	void presentation::createSwapchain(VkDevice device, uint32_t graphicsFamilyIndex, uint32_t presentFamilyIndex, PresentationInfo* pInfo) {
		VkSurfaceCapabilitiesKHR surfaceCapabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDeviceCandidate, surfaceCandidate, &surfaceCapabilities);

		uint32_t l_imageCount = surfaceCapabilities.minImageCount + 1;
		if (surfaceCapabilities.maxImageCount && l_imageCount > surfaceCapabilities.maxImageCount)
			l_imageCount = surfaceCapabilities.maxImageCount;

		uint32_t queueFamilyIndices[] = { graphicsFamilyIndex, presentFamilyIndex };

		bool concurrentImageUse = (graphicsFamilyIndex != presentFamilyIndex);

		VkExtent2D l_extent = surfaceCapabilities.currentExtent;
		VkSurfaceTransformFlagBitsKHR l_transform = surfaceCapabilities.currentTransform;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.surface = surfaceCandidate;
		createInfo.minImageCount = l_imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = l_extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = concurrentImageUse ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = concurrentImageUse ? 2 : 0;
		createInfo.pQueueFamilyIndices = concurrentImageUse ? queueFamilyIndices : nullptr;
		createInfo.preTransform = l_transform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = swapchain;

		VkResult result = swapchain.create(device, &createInfo);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: swapchain_creation!");

		if (!pInfo)
			return;

		pInfo->swapchain = swapchain;
		pInfo->swapchainImageCount = createInfo.minImageCount;
		pInfo->swapchainImageFormat = createInfo.imageFormat;
		pInfo->swapchainImageExtent = createInfo.imageExtent;
	}

	void presentation::reset() noexcept {
		swapchain.reset();
	}

}
