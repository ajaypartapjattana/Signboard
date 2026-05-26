#include "Signboard/RHI/Internal/rhi_pAccess.h"

#include <algorithm>

namespace rhi {

	pcdSwapchainCreate::pcdSwapchainCreate(const device& device, const surface& surface, VkSwapchainCreateInfoKHR* pCreateInfo) noexcept
		: 
		r_device(_pAccess::extract(device)), 
		m_phys(_pAccess::physicalDevice(device)), 
		r_surface(_pAccess::extract(surface)),

		pInfo(allot_basic(pCreateInfo))
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_phys, r_surface, &surface_caps);
	}

	VkSwapchainCreateInfoKHR* pcdSwapchainCreate::allot_basic(VkSwapchainCreateInfoKHR* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkSwapchainCreateInfoKHR>();
		
		VkSwapchainCreateInfoKHR* info = m_ownedInfo.get();
		info->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

		info->imageArrayLayers = 1;
		info->imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		info->clipped = VK_TRUE;
		info->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		return info;
	}

	void pcdSwapchainCreate::carry_surface() noexcept {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_phys, r_surface, &surface_caps);

		pInfo->imageExtent = surface_caps.currentExtent;
		pInfo->preTransform = surface_caps.currentTransform;
	}

	VkResult pcdSwapchainCreate::imageFormat(VkFormat format, VkColorSpaceKHR colorSpace) noexcept {
#ifdef _VALIDATE
		uint32_t _expFormatCt = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_phys, r_surface, &_expFormatCt, nullptr);
		std::vector<VkSurfaceFormatKHR> available_surfaceFormat(_expFormatCt);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_phys, r_surface, &_expFormatCt, available_surfaceFormat.data());

		if (_expFormatCt == 1 && available_surfaceFormat[0].format == VK_FORMAT_UNDEFINED) {
			pInfo->imageFormat = format;
			pInfo->imageColorSpace = colorSpace;

			return VK_SUCCESS;
		}

		bool _fAv = false;
		for (const VkSurfaceFormatKHR surfaceFormat : available_surfaceFormat) {
			if (format == surfaceFormat.format && colorSpace == surfaceFormat.colorSpace) {
				_fAv = true;
				break;
			}
		}

		if (!_fAv)
			return VK_ERROR_FORMAT_NOT_SUPPORTED;

#endif

		pInfo->imageFormat = format;
		pInfo->imageColorSpace = colorSpace;
		
		return VK_SUCCESS;
	}

	VkResult pcdSwapchainCreate::transform(VkSurfaceTransformFlagBitsKHR transform) {
#ifdef _VALIDATE
		if (!(surface_caps.supportedTransforms & transform))
			return VK_ERROR_FEATURE_NOT_PRESENT;
#endif

		pInfo->preTransform = transform;

		return VK_SUCCESS;
	}

	VkResult pcdSwapchainCreate::presentMode(VkPresentModeKHR mode) noexcept {
#ifdef _VALIDATE
		uint32_t _expModeCt = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_phys, r_surface, &_expModeCt, nullptr);
		std::vector<VkPresentModeKHR> available_presentMode(_expModeCt);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_phys, r_surface, &_expModeCt, available_presentMode.data());

		bool _mAv = false;
		for (const VkPresentModeKHR presentMode : available_presentMode) {
			if (mode == presentMode) {
				_mAv = true;
				break;
			}
		}

		if (!_mAv)
			return VK_INCOMPLETE;
#endif

		pInfo->presentMode = mode;

		return VK_SUCCESS;
	}

	void pcdSwapchainCreate::imageExtent(uint32_t w, uint32_t h) noexcept {
		pInfo->imageExtent.width = std::clamp(w, surface_caps.minImageExtent.width, surface_caps.maxImageExtent.width);
		pInfo->imageExtent.height = std::clamp(h, surface_caps.minImageExtent.height, surface_caps.maxImageExtent.height);
	}

	void pcdSwapchainCreate::imageUsage(VkImageUsageFlags usage) noexcept {
		pInfo->imageUsage = usage;
	}

	void pcdSwapchainCreate::arraylayers(uint32_t layerCount) noexcept {
		pInfo->imageArrayLayers = layerCount;
	}

	void pcdSwapchainCreate::enablePreserve(bool enable) noexcept {
		pInfo->clipped = enable ? VK_FALSE : VK_TRUE;
	}

	VkResult pcdSwapchainCreate::imageCount(uint32_t count) noexcept {
#ifdef _VALIDATE
		if (count < surface_caps.minImageCount) {
			return VK_ERROR_NOT_PERMITTED;
		}
		else if (surface_caps.maxImageCount != 0 && count > surface_caps.maxImageCount) {
			return VK_ERROR_NOT_PERMITTED;
		}
#endif

		pInfo->minImageCount = count;

		return VK_SUCCESS;
	}

	void pcdSwapchainCreate::recycle_swapchain(const creSwapchain& swapchain) noexcept {
		pInfo->oldSwapchain = swapchain.m_swapchain;
	}

	VkResult pcdSwapchainCreate::publish(creSwapchain& target) const noexcept {
		VkSwapchainKHR _swapchain;
		VkResult result = vkCreateSwapchainKHR(r_device, pInfo, nullptr, &_swapchain);
		if (result != VK_SUCCESS)
			return result;

		if (target.m_swapchain)
			vkDestroySwapchainKHR(r_device, target.m_swapchain, nullptr);

		VkSwapchainCreateInfoKHR& info = *pInfo;

		target.m_swapchain = _swapchain;
		target.format = info.imageFormat;
		target.extent = { info.imageExtent.width, info.imageExtent.height };
		target.r_device = r_device;

		return VK_SUCCESS;
	}

	void pcdSwapchainCreate::preset(VkSwapchainCreateInfoKHR* pCreateInfo) noexcept {
		m_ownedInfo.reset();
		pInfo = allot_basic(pCreateInfo);
	}

	void pcdSwapchainCreate::reset() noexcept {
		m_ownedInfo.reset();
		pInfo = allot_basic(nullptr);
	}

}