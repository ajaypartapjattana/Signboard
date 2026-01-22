#include "swapchain_builder.h"

#include "Signboard/RHI/core/swapchain.h"
#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/core/surface_vk.h"

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