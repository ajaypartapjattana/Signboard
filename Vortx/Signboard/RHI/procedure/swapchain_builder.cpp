#include "swapchain_builder.h"

#include "Signboard/RHI/core/swapchain_vk.h"
#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/core/surface_vk.h"

namespace rhi::procedure {

	struct swapchain_builder::Impl {
		VkDevice m_device;
		VkPhysicalDevice m_phys;
		VkSurfaceKHR m_surface;

		VkSurfaceCapabilitiesKHR surface_caps;
		std::vector<VkSurfaceFormatKHR> available_surfaceFormat;
		std::vector<VkPresentModeKHR> available_presentMode;
	};

	swapchain_builder::swapchain_builder(const rhi::core::device& device, const rhi::core::surface& surface)
		: impl(new Impl) 
	{
		Impl& i = *impl;
		i.m_device = rhi::core::device_vkAccess::get(device);
		i.m_phys = rhi::core::device_vkAccess::get_physicalDevice(device);
		i.m_surface = rhi::core::surface_vkAccess::get(surface);
	}

	void swapchain_builder::query_support() {
		Impl& i = *impl;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(i.m_phys, i.m_surface, &i.surface_caps);

		uint32_t count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(i.m_phys, i.m_surface, &count, nullptr);
		i.available_surfaceFormat.resize(count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(i.m_phys, i.m_surface, &count, i.available_surfaceFormat.data());

		count = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(i.m_phys, i.m_surface, &count, nullptr);
		i.available_presentMode.resize(count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(i.m_phys, i.m_surface, &count, i.available_presentMode.data());
	}

	swapchain_builder& swapchain_builder::prefer_format() {
		//how do we get the format from callee?
		//should we delay support query?
	}



}