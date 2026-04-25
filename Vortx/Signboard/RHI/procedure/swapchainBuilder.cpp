#include "swapchainBuilder.h"

#include "Signboard/RHI/primitive/swapchain.h"
#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/core/surface_pAccess.h"

#include <algorithm>

namespace rhi {

	pcdSwapchainCreate::pcdSwapchainCreate(const rhi::creDevice& device, const rhi::creSurface& surface, VkSwapchainCreateInfoKHR* pCreateInfo) noexcept
		: 
		r_device(rhi::access::device_pAccess::extract(device)), 
		m_phys(rhi::access::device_pAccess::get_physicalDevice(device)), 
		r_surface(rhi::access::surface_pAccess::extract(surface)),

		_info(fetch_basic(pCreateInfo))
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_phys, r_surface, &surface_caps);
	}

	VkSwapchainCreateInfoKHR pcdSwapchainCreate::fetch_basic(VkSwapchainCreateInfoKHR* pCreateInfo) const noexcept {
		if (pCreateInfo)
			return *pCreateInfo;

		VkSwapchainCreateInfoKHR info{};
		info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		info.surface = r_surface;
		info.imageArrayLayers = 1;
		info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.clipped = VK_TRUE;
		info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		return info;
	}

	void pcdSwapchainCreate::carry_surface() noexcept {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_phys, r_surface, &surface_caps);

		_info.imageExtent = surface_caps.currentExtent;
		_info.preTransform = surface_caps.currentTransform;
	}

	VkResult pcdSwapchainCreate::set_imageFormat(VkFormat format, VkColorSpaceKHR colorSpace) noexcept {
#ifndef NDEBUG
		uint32_t _expFormatCt = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_phys, r_surface, &_expFormatCt, nullptr);
		std::vector<VkSurfaceFormatKHR> available_surfaceFormat(_expFormatCt);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_phys, r_surface, &_expFormatCt, available_surfaceFormat.data());

		if (_expFormatCt == 1 && available_surfaceFormat[0].format == VK_FORMAT_UNDEFINED) {
			_info.imageFormat = format;
			_info.imageColorSpace = colorSpace;

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

		_info.imageFormat = format;
		_info.imageColorSpace = colorSpace;
		
		return VK_SUCCESS;
	}

	VkResult pcdSwapchainCreate::set_transform(VkSurfaceTransformFlagBitsKHR transform) {
#ifndef NDEBUG
		if (!(surface_caps.supportedTransforms & transform))
			return VK_ERROR_FEATURE_NOT_PRESENT;
#endif

		_info.preTransform = transform;

		return VK_SUCCESS;
	}

	VkResult pcdSwapchainCreate::set_presentMode(VkPresentModeKHR mode) noexcept {
#ifndef NDEBUG
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

		_info.presentMode = mode;

		return VK_SUCCESS;
	}

	void pcdSwapchainCreate::set_extent(uint32_t w, uint32_t h) noexcept {
		_info.imageExtent.width = std::clamp(w, surface_caps.minImageExtent.width, surface_caps.maxImageExtent.width);
		_info.imageExtent.height = std::clamp(h, surface_caps.minImageExtent.height, surface_caps.maxImageExtent.height);

		return;
	}

	VkResult pcdSwapchainCreate::set_imageCount(uint32_t count) noexcept {
#ifndef NDEBUG
		if (count < surface_caps.minImageCount) {
			return VK_ERROR_NOT_PERMITTED;
		}
		else if (surface_caps.maxImageCount != 0 && count > surface_caps.maxImageCount) {
			return VK_ERROR_NOT_PERMITTED;
		}
#endif

		_info.minImageCount = count;

		return VK_SUCCESS;
	}

	void pcdSwapchainCreate::recycle_swapchain(const rhi::pmvSwapchain& swapchain) noexcept {
		_info.oldSwapchain = swapchain.m_swapchain;
	}

	VkResult pcdSwapchainCreate::publish(rhi::pmvSwapchain& target) const noexcept {
		if (target.m_swapchain)
			vkDestroySwapchainKHR(r_device, target.m_swapchain, nullptr);

		VkResult result = vkCreateSwapchainKHR(r_device, &_info, nullptr, &target.m_swapchain);
		if (result != VK_SUCCESS)
			return result;

		target.r_device = r_device;

		return VK_SUCCESS;
	}

	void pcdSwapchainCreate::preset(VkSwapchainCreateInfoKHR* pCreateInfo) noexcept {
		_info = fetch_basic(pCreateInfo);

	}

	void pcdSwapchainCreate::reset() noexcept {
		_info = fetch_basic(nullptr);
	}

}