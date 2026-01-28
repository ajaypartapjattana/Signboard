#include "swapchain_builder.h"

#include "Signboard/RHI/core/swapchain.h"
#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/core/surface_vk.h"

#include "bitops.h"

#include <stdexcept>
#include <algorithm>

namespace rhi::procedure {

	swapchain_builder::swapchain_builder(const rhi::core::device& device, const rhi::core::surface& surface) {
		m_device = rhi::core::device_vkAccess::get(device);
		m_phys = rhi::core::device_vkAccess::get_physicalDevice(device);
		m_surface = rhi::core::surface_vkAccess::get(surface);

		query_support();
	}

	void swapchain_builder::query_support() {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_phys, m_surface, &surface_caps);

		uint32_t count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_phys, m_surface, &count, nullptr);
		available_surfaceFormat.resize(count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_phys, m_surface, &count, available_surfaceFormat.data());

		count = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_phys, m_surface, &count, nullptr);
		available_presentMode.resize(count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_phys, m_surface, &count, available_presentMode.data());
	}

	swapchain_builder& swapchain_builder::prefer_format_srgb() {
		return prefer_format(VK_FORMAT_R8G8B8A8_SRGB);
	}

	swapchain_builder& swapchain_builder::prefer_format_unorm() {
		return prefer_format(VK_FORMAT_R8G8B8A8_UNORM);
	}

	swapchain_builder& swapchain_builder::prefer_presentMode_MAILBOX() {
		return prefer_presentMode(VK_PRESENT_MODE_MAILBOX_KHR);
	}

	swapchain_builder& swapchain_builder::prefer_presentMode_IMMEDIATE() {
		return prefer_presentMode(VK_PRESENT_MODE_IMMEDIATE_KHR);
	}

	swapchain_builder& swapchain_builder::set_extent(uint32_t w, uint32_t h) {
		if (surface_caps.currentExtent.width != UINT32_MAX) {
			final_extent = surface_caps.currentExtent;
		} else {
			final_extent.width = std::clamp(w, surface_caps.minImageExtent.width, surface_caps.maxImageExtent.width);
			final_extent.height = std::clamp(h, surface_caps.minImageExtent.height, surface_caps.maxImageExtent.height);
		}

		extent_chosen = true;
		return *this;
	}

	swapchain_builder& swapchain_builder::set_imageCount(uint32_t count) {
		uint32_t min = surface_caps.minImageCount;
		uint32_t max = surface_caps.maxImageCount;

		if (count < min) {
			final_imageCount = min;
		} else if (max != 0 && count > max) {
			final_imageCount = max;
		} else {
			final_imageCount = count;
		}

		imageCount_chosen = true;
		return *this;
	}

	swapchain_builder& swapchain_builder::allow_tearing(bool b) {
		if (b) {
			prefer_presentMode(VK_PRESENT_MODE_IMMEDIATE_KHR);
		} else {
			prefer_presentMode(VK_PRESENT_MODE_FIFO_KHR);
		}

		return *this;
	}

	rhi::core::swapchain swapchain_builder::build(VkAllocationCallbacks* allocator) {
		if (available_surfaceFormat.empty() || available_presentMode.empty())
			throw std::runtime_error("surface does not support swapchains!");

		if (!format_chosen)
			final_format = available_surfaceFormat[0];

		if (!presentMode_chosen)
			final_presentMode = VK_PRESENT_MODE_FIFO_KHR;

		if (!extent_chosen)
			set_extent(surface_caps.currentExtent.width, surface_caps.currentExtent.height);

		if (!imageCount_chosen)
			set_imageCount(surface_caps.minImageCount + 1);

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_surface;

		createInfo.minImageCount = final_imageCount;
		createInfo.imageFormat = final_format.format;
		createInfo.imageColorSpace = final_format.colorSpace;
		createInfo.imageExtent = final_extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		createInfo.preTransform = surface_caps.currentTransform;
		createInfo.compositeAlpha = (surface_caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) ? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR : static_cast<VkCompositeAlphaFlagBitsKHR>( 1u << rhi::procedure::bitops::ctz(surface_caps.supportedCompositeAlpha));
		createInfo.presentMode = final_presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = recycled_swapchain;

		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;

		VkSwapchainKHR vk_swapchain = VK_NULL_HANDLE;
		if (vkCreateSwapchainKHR(m_device, &createInfo, allocator, &vk_swapchain) != VK_SUCCESS)
			throw std::runtime_error("failed to create swapchain!");

		rhi::core::swapchain l_swapchain;
		l_swapchain.m_swapchain = vk_swapchain;
		l_swapchain.m_format = final_format.format;
		l_swapchain.m_device = m_device;

		return l_swapchain;
	}

	swapchain_builder& swapchain_builder::recycle_swapchain(const rhi::core::swapchain& sc) {
		recycled_swapchain = sc.m_swapchain;
		return *this;
	}

	swapchain_builder& swapchain_builder::prefer_format(VkFormat format) {
		if (available_surfaceFormat.size() == 1 && available_surfaceFormat[0].format == VK_FORMAT_UNDEFINED) {
			final_format.format = format;
			final_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			format_chosen = true;

			return *this;
		}

		for (const auto& sf : available_surfaceFormat) {
			if (sf.format == format) {
				final_format = sf;
				format_chosen = true;

				return *this;
			}
		}

		final_format = available_surfaceFormat[0];
		format_chosen = true;
		return *this;
	}

	swapchain_builder& swapchain_builder::prefer_presentMode(VkPresentModeKHR presentMode) {
		for (const VkPresentModeKHR& pm : available_presentMode) {
			if (pm == presentMode) {
				final_presentMode = pm;
				presentMode_chosen = true;

				return *this;
			}
		}

		final_presentMode = VK_PRESENT_MODE_FIFO_KHR;
		presentMode_chosen = true;
		
		return *this;
	}

}