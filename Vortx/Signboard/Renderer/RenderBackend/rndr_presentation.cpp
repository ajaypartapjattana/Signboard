#include "rndr_presentation.h"

#include "rndr_context_Access.h"

rndr_presentation::rndr_presentation(const rndr_context& ctx, const uint32_t bufferedFrame_count )
	: 
	r_device(rndr_context_Access::get_device(ctx)),
	r_surface(rndr_context_Access::get_surface(ctx)),

	m_watchdog(r_device),

	m_swapchain()
{
	construct_swapchain();
}

void rndr_presentation::update_bufferedFrameCount(const uint32_t count) {
	uint32_t clampedFrames = std::max(2u, bufferedFrame_count);
	if (clampedFrames == bufferedFrame_count)
		return;

	bufferedFrame_count = clampedFrames;

	m_watchdog.wait_device();
	construct_swapchain();
}

void rndr_presentation::recreate_swapchain() {
	m_watchdog.wait_device();
	construct_swapchain();
}

VkResult rndr_presentation::construct_swapchain() {
	rhi::procedure::swapchain_builder prcdr{ r_device, r_surface };

	prcdr.prefer_format(VK_FORMAT_B8G8R8A8_SRGB);
	prcdr.set_imageCount(bufferedFrame_count);
	prcdr.recycle_swapchain(m_swapchain);

	return prcdr.build(m_swapchain);
}