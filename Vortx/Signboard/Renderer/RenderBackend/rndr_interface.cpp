#include "rndr_interface.h"

#include "rndr_context_Access.h"
#include "rndr_presentation_Access.h"

#include <algorithm>
#include <stdexcept>

rndr_interface::rndr_interface(const rndr_context& context, const rndr_presentation& presentation)
	: 
	r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),

	m_swapchainHandler(r_device)
{
	summon_commandPools();

	configure_bufferedFrames();
}

rndr_interface::frame::frame(const rhi::core::device& device)
	: 
	image_available(device),
	render_finished(device),
	in_flight(device, true)
{

}

uint32_t rndr_interface::acquire_toWriteImage() const noexcept {
	uint32_t a_imageIndex;
	m_swapchainHandler.acquire_freeSwapchainImage(&frames[activeFrameIndex].image_available, a_imageIndex);

	return a_imageIndex;
}

VkResult rndr_interface::summon_commandPools() {
	rhi::procedure::commandPool_creator prcdr{ r_device };
	return prcdr.create(m_commandPools);
}

void rndr_interface::configure_bufferedFrames() {
	m_swapchainHandler.set_swapchain(r_swapchain);

	bufferedFrame_count = r_swapchain.native_imageCount();

	frames.reserve(bufferedFrame_count);
	for (uint32_t i = 0; i < bufferedFrame_count; ++i)
		frames.emplace_back(r_device);

	allocate_renderCommandBuffers();
}

void rndr_interface::allocate_renderCommandBuffers() {
	release_renderCommandBuffers();

	if (m_graphicsPoolIndex == UINT32_MAX)
		return;

	rhi::procedure::commandBuffer_allocator prcdr{ r_device };

	for (uint32_t i = 0; i < bufferedFrame_count; i++) {
		prcdr.allocate(m_commandPools.graphicsPool, &frames[i].cmd, 1);
	}
}

void rndr_interface::release_renderCommandBuffers() noexcept {
	// --- incomplete here! [add free/release capability to the commandBuffer_allocator before proceeding->]
}

rhi::primitive::commandBuffer& rndr_interface::get_activeFrame_cmd() {
	return frames[activeFrameIndex].cmd;
}

void rndr_interface::submit_activeFrame_cmd() {

}

void rndr_interface::advance_frame() noexcept {
	activeFrameIndex = (activeFrameIndex + 1) % bufferedFrame_count;
}
