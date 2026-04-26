#include "rndr_presentation.h"

#include "Signboard/Renderer/Context/render_context_Access.h"
#include "Signboard/Resources/resources_pAccess.h"

rndr_presentation::rndr_presentation(const RHIContext& ctx, Resources& resources, const uint32_t bufferedFrame_count)
	:
	r_device(rndr_context_Access::get_device(ctx)),
	r_surface(rndr_context_Access::get_surface(ctx)),
	r_resources(resources),

	bufferedFrame_count(bufferedFrame_count),

	m_watchdog(r_device),
	m_swapchainCreate(r_device, r_surface),

	m_swapchain()
{
	m_swapchainCreate.set_imageFormat(VK_FORMAT_B8G8R8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR);
	m_swapchainCreate.set_imageCount(bufferedFrame_count);
	m_swapchainCreate.set_transform(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);
	m_swapchainCreate.carry_surface();

	construct_swapchain();
}

void rndr_presentation::recreate_swapchain(uint32_t* imageCount) {
	m_watchdog.wait_device();

	if (imageCount) {
		bufferedFrame_count = std::max(2u, *imageCount);
		m_swapchainCreate.set_imageCount(bufferedFrame_count);
	}
	m_swapchainCreate.carry_surface();
	construct_swapchain();
}

VkResult rndr_presentation::construct_swapchain() {
	m_swapchainCreate.recycle_swapchain(m_swapchain);

	VkResult result = m_swapchainCreate.publish(m_swapchain);
	if (result != VK_SUCCESS)
		return result;

	rhi::pcdSwapchainImageAllocate pcd{ r_device, m_swapchain };

	auto _ctor = [&](uint32_t i, rhi::pmvImage* image) {
		pcd.publish(*image, i);
	};

	uint32_t _siCt = pcd.get_imageCount();

	ctnr::vltWrite<rhi::pmvImage> _wrt = Resources_pAccess::get_imageWrite(r_resources);

	for (uint32_t idx : swapchainImageHandles) {
		_wrt.destroy(idx);
	}

	swapchainImageHandles = _wrt.construct_many(_siCt, _ctor);

	return VK_SUCCESS;
}

uint32_t rndr_presentation::expose_swapchainImageCount() const noexcept {
	return static_cast<uint32_t>(swapchainImageHandles.size());
}

ctnr::span<const uint32_t> rndr_presentation::expose_swapchainImages() const noexcept {
	return swapchainImageHandles;
}