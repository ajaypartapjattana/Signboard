#include "presentation.h"

#include "Signboard/Core/Math/search.h"
#include <array>
#include <vector>

namespace rndr {

	constexpr std::array preferredSurfaceFormats = {
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

	struct SurfaceFormatEq {
		bool operator()(const VkSurfaceFormatKHR& a, const VkSurfaceFormatKHR& b) const noexcept {
			return a.format == b.format && a.colorSpace == b.colorSpace;
		}
	};

	constexpr std::array preferredPresentModes = {
		VK_PRESENT_MODE_MAILBOX_KHR,
		VK_PRESENT_MODE_FIFO_KHR ,
		VK_PRESENT_MODE_IMMEDIATE_KHR
	};

	VkResult presentationStage::createPresentationConfiguration(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* presentFamilyIndex, uint32_t selectedGraphicsFamilyIndex = UINT32_MAX) noexcept {
		uint32_t presentationFamilyIndex = UINT32_MAX;
		{
			uint32_t familyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilyProperties(familyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, queueFamilyProperties.data());

			std::vector<VkBool32> presentationSupport(familyCount, VK_FALSE);
			for (uint32_t i = 0; i < familyCount; ++i) {
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentationSupport[i]);
			}

			if (selectedGraphicsFamilyIndex != UINT32_MAX && presentationSupport[selectedGraphicsFamilyIndex]) {
				presentationFamilyIndex = selectedGraphicsFamilyIndex;
			}

			if (presentationFamilyIndex == UINT32_MAX) {
				for (uint32_t i = 0; i < familyCount; ++i) {
					if (!presentationSupport[i])
						continue;

					if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
						presentationFamilyIndex = i;
						break;
					}

					if (presentationFamilyIndex == UINT32_MAX) {
						presentationFamilyIndex = i;
					}
				}
			}

			if (presentationFamilyIndex == UINT32_MAX)
				return VK_ERROR_FEATURE_NOT_PRESENT;

			*presentFamilyIndex = presentationFamilyIndex;
		}

		VkSurfaceFormatKHR surfaceFormat;

		{
			uint32_t supportedFormatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatCount, nullptr);

			std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatCount, supportedFormats.data());

			size_t index = 0;
			std::findEarly_basic<VkSurfaceFormatKHR, SurfaceFormatEq>(preferredSurfaceFormats.data(), preferredSurfaceFormats.size(), supportedFormats.data(), supportedFormats.size(), &index);

			surfaceFormat = supportedFormats[index];
		}

		VkPresentModeKHR presentMode;

		{
			uint32_t supportedPresentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &supportedPresentModeCount, nullptr);

			std::vector<VkPresentModeKHR> supportedPresentModes(supportedPresentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &supportedPresentModeCount, supportedPresentModes.data());

			size_t index = 0;
			std::findEarly_basic(preferredPresentModes.data(), preferredPresentModes.size(), supportedPresentModes.data(), supportedPresentModes.size(), &index);

			presentMode = supportedPresentModes[index];
		}

		presentationFamily = presentationFamilyIndex;

		compatible.physicalDevice = physicalDevice;
		compatible.surface = surface;
		compatible.surfaceFormat = surfaceFormat;
		compatible.surfacePresentMode = presentMode;

		return VK_SUCCESS;
	}

	VkResult presentationStage::createSwapchain(VkDevice device, uint32_t graphicsFamilyIndex) noexcept {
		VkSurfaceCapabilitiesKHR surfaceCapabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(compatible.physicalDevice, compatible.surface, &surfaceCapabilities);

		uint32_t l_imageCount = surfaceCapabilities.minImageCount + 1;
		if (surfaceCapabilities.maxImageCount && l_imageCount > surfaceCapabilities.maxImageCount)
			l_imageCount = surfaceCapabilities.maxImageCount;

		VkExtent2D l_extent = surfaceCapabilities.currentExtent;
		VkSurfaceTransformFlagBitsKHR l_transform = surfaceCapabilities.currentTransform;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.surface = compatible.surface;
		createInfo.minImageCount = l_imageCount;
		createInfo.imageFormat = compatible.surfaceFormat.format;
		createInfo.imageColorSpace = compatible.surfaceFormat.colorSpace;
		createInfo.imageExtent = l_extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
		createInfo.preTransform = l_transform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = compatible.surfacePresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		std::array queueFamilyIndices{ graphicsFamilyIndex, presentationFamily };

		if (graphicsFamilyIndex != presentationFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
		}

		swapchain.root(device);
		
		VkResult result = swapchain.create(&createInfo);
		if (result != VK_SUCCESS)
			return result;

		uint32_t swapchainImageCount;
		vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);

		swapchainState.imageCount = swapchainImageCount;
		swapchainState.extent = createInfo.imageExtent;
		
		return VK_SUCCESS;
	}

}