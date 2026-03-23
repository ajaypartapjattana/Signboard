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

rndr_interface::~rndr_interface() noexcept {
	m_watchdog.wait_device();
}

rndr_interface::frame::frame(const rhi::core::device& device)
	: 
	imageAvailable(device),
	frameInFlight(device, true)
{

}

VkResult rndr_interface::summon_commandPools() {
	rhi::procedure::commandPool_creator prcdr{ r_device };
	return prcdr.create(m_commandPools);
}

void rndr_interface::validate_swapchainDependancy() {
	rhi::procedure::swapchain_handler l_handler{ r_device, r_swapchain };
	m_swapchainHandler = std::move(l_handler);

	rhi::procedure::swapchain_presenter l_presenter{ r_device, r_swapchain };
	m_presenter = std::move(l_presenter);
}

void rndr_interface::configure_bufferedFrames() {
	bufferedFrame_count = r_swapchain.native_imageCount();
	
	imagesInFlight.resize(bufferedFrame_count, -1);

	frames.clear();
	frames.reserve(bufferedFrame_count);
	for (uint32_t i = 0; i < bufferedFrame_count; ++i)
		frames.emplace_back(r_device);

	imageRenderFinished.clear();
	imageRenderFinished.reserve(bufferedFrame_count);
	for (uint32_t i = 0; i < bufferedFrame_count; ++i)
		imageRenderFinished.emplace_back(r_device);

	allocate_renderCommandBuffers();
}

void rndr_interface::allocate_renderCommandBuffers() {
	rhi::procedure::commandBuffer_allocator prcdr{ r_device };

	for (uint32_t i = 0; i < bufferedFrame_count; i++) {
		prcdr.allocate(m_commandPools.graphicsPool, &frames[i].cmd, 1);
	}
}

uint32_t rndr_interface::acquire_toWriteImage(VkBool32* acquire_optimal) noexcept {
	m_watchdog.watch_fence(frames[activeFrameIndex].frameInFlight);

	VkResult result = m_swapchainHandler.acquire_freeSwapchainImage(&frames[activeFrameIndex].imageAvailable, a_imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		*acquire_optimal = VK_FALSE;
		return 0;
	}

	*acquire_optimal = (result == VK_SUCCESS);

	int32_t frameIndex = imagesInFlight[a_imageIndex];
	if (frameIndex != -1) {
		m_watchdog.watch_fence(frames[frameIndex].frameInFlight);
	}

	imagesInFlight[a_imageIndex] = activeFrameIndex;

	return a_imageIndex;
}

rhi::primitive::commandBuffer& rndr_interface::get_activeFrame_cmd() {
	return frames[activeFrameIndex].cmd;
}

void rndr_interface::submit_activeFrame_cmd() {
	VkPipelineStageFlags waitStage[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	m_graphics_submission.update_toWait_semaphores(&frames[activeFrameIndex].imageAvailable, waitStage, 1);
	m_graphics_submission.update_toSignal_semaphores(&imageRenderFinished[a_imageIndex], 1);
	m_graphics_submission.set_toTrigger_fence(frames[activeFrameIndex].frameInFlight);

	m_graphics_submission.update_toSubmit_cmd(&frames[activeFrameIndex].cmd, 1);

	m_watchdog.reset_fence(frames[activeFrameIndex].frameInFlight);
	m_graphics_submission.submit_graphics_cmd();
}

void rndr_interface::present_activeFrame(uint32_t toPresent_Image) {
	m_presenter.update_toWait_semaphores(&imageRenderFinished[a_imageIndex], 1);
	m_presenter.present(toPresent_Image);
}

void rndr_interface::advance_frame() noexcept {
	activeFrameIndex = (activeFrameIndex + 1) % bufferedFrame_count;
}
