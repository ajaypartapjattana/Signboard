#include "rndr_interface.h"

#include "rndr_context_Access.h"
#include "rndr_presentation_Access.h"

#include <algorithm>
#include <stdexcept>

rndr_interface::rndr_interface(const rndr_context& context, const rndr_presentation& presentation)
	: 
	r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),

	m_watchdog(r_device),
	m_swapchainHandler(r_device, r_swapchain),
	m_presenter(r_device, r_swapchain),

	m_graphics_submission(r_device)
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

uint32_t rndr_interface::acquire_toWriteImage() noexcept {
	m_watchdog.watch(frames[activeFrameIndex].in_flight);

	uint32_t a_imageIndex;
	m_swapchainHandler.acquire_freeSwapchainImage(&frames[activeFrameIndex].image_available, a_imageIndex);

	return a_imageIndex;
}

VkResult rndr_interface::summon_commandPools() {
	rhi::procedure::commandPool_creator prcdr{ r_device };
	return prcdr.create(m_commandPools);
}

void rndr_interface::configure_bufferedFrames() {
	bufferedFrame_count = r_swapchain.native_imageCount();

	frames.reserve(bufferedFrame_count);
	for (uint32_t i = 0; i < bufferedFrame_count; ++i)
		frames.emplace_back(r_device);

	allocate_renderCommandBuffers();
}

void rndr_interface::allocate_renderCommandBuffers() {
	release_renderCommandBuffers();

	rhi::procedure::commandBuffer_allocator prcdr{ r_device };

	for (uint32_t i = 0; i < bufferedFrame_count; i++) {
		prcdr.allocate(m_commandPools.graphicsPool, &frames[i].cmd, 1);
	}
}

void rndr_interface::release_renderCommandBuffers() noexcept {
	// --- todo [add free/release capability to the commandBuffer_allocator->]
}

rhi::primitive::commandBuffer& rndr_interface::get_activeFrame_cmd() {
	return frames[activeFrameIndex].cmd;
}

void rndr_interface::submit_activeFrame_cmd() {
	VkPipelineStageFlags waitStage[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	m_graphics_submission.update_toWait_semaphores(&frames[activeFrameIndex].image_available, waitStage, 1);
	m_graphics_submission.update_toSignal_semaphores(&frames[activeFrameIndex].render_finished, 1);
	m_graphics_submission.set_toTrigger_fence(frames[activeFrameIndex].in_flight);

	m_graphics_submission.update_toSubmit_cmd(&frames[activeFrameIndex].cmd, 1);

	m_graphics_submission.submit_graphics_cmd();
}

void rndr_interface::present_activeFrame(uint32_t toPresent_Image) {
	m_presenter.update_toWait_semaphores(&frames[activeFrameIndex].render_finished, 1);
	m_presenter.present(toPresent_Image);
}

void rndr_interface::advance_frame() noexcept {
	activeFrameIndex = (activeFrameIndex + 1) % bufferedFrame_count;
}
