#include "rndr_interface.h"

#include "Signboard/Renderer/Context/render_context_Access.h"
#include "rndr_presentation_Access.h"

#include <algorithm>
#include <stdexcept>

rndr_interface::rndr_interface(const RHIContext& context, const rndr_presentation& presentation)
	: 
	r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),

	m_watchdog(r_device),
	m_swapchainHandler(r_device, r_swapchain),
	m_presenter(r_device, r_swapchain),

	m_graphics_submission(r_device),
	m_tranfer_submission(r_device)
{
	summon_commandPools();
	configure_bufferedFrames(presentation.expose_swapchainImageCount());
}

rndr_interface::~rndr_interface() noexcept {
	m_watchdog.wait_device();
}

rndr_interface::frame::frame(const rhi::creDevice& device)
	:
	frameInFlight(device, true),
	imageAvailable(device),
	transferFinished(device)
{

}

VkResult rndr_interface::summon_commandPools() {
	rhi::pcdCommandPoolCreator prcdr{ r_device };
	return prcdr.create(m_commandPools);
}

void rndr_interface::validate_swapchainDependancy() {
	rhi::pcdSwapchainHandler l_handler{ r_device, r_swapchain };
	m_swapchainHandler = std::move(l_handler);

	rhi::pcdSwapchainPresenter l_presenter{ r_device, r_swapchain };
	m_presenter = std::move(l_presenter);
}

void rndr_interface::configure_bufferedFrames(uint32_t count) {
	bufferedFrame_count = count;
	
	imagesInFlight.resize(bufferedFrame_count, -1);

	frames.clear();
	frames.reserve(bufferedFrame_count);
	for (uint32_t i = 0; i < bufferedFrame_count; ++i)
		frames.emplace_back(r_device);

	imageRenderFinished.clear();
	imageRenderFinished.reserve(bufferedFrame_count);
	for (uint32_t i = 0; i < bufferedFrame_count; ++i)
		imageRenderFinished.emplace_back(r_device);

	allocate_command_buffers();
}

void rndr_interface::allocate_command_buffers() {
	rhi::pcdCommandBufferAllocator prcdr{ r_device };

	for (uint32_t i = 0; i < bufferedFrame_count; i++) {
		prcdr.allocate(m_commandPools.graphicsPool, &frames[i].CMDGraphics, 1);
		prcdr.allocate(m_commandPools.transferPool, &frames[i].CMDTransfer, 1);
	}
}

uint32_t rndr_interface::acquire_toWriteImage(VkBool32* acquire_optimal) noexcept {
	m_watchdog.watch_fence(frames[activeFrameIndex].frameInFlight);

	VkResult result = m_swapchainHandler.acquire_freeSwapchainImage(&frames[activeFrameIndex].imageAvailable, a_imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
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

rhi::pmvCommandBuffer& rndr_interface::get_graphicsCMD() {
	return frames[activeFrameIndex].CMDGraphics;
}

rhi::pmvCommandBuffer& rndr_interface::get_transferCMD() {
	return frames[activeFrameIndex].CMDTransfer;
}

void rndr_interface::submit_graphics(bool waitTransfer) {
	const frame& currentFrame = frames[activeFrameIndex];

	m_graphics_submission.reset();

	m_graphics_submission.add_wait(currentFrame.imageAvailable, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

	if (waitTransfer)
		m_graphics_submission.add_wait(currentFrame.transferFinished, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);

	m_graphics_submission.add_signal(imageRenderFinished[a_imageIndex]);
	m_graphics_submission.set_toTrigger_fence(currentFrame.frameInFlight);

	m_graphics_submission.add_cmd(currentFrame.CMDGraphics);

	m_watchdog.reset_fence(currentFrame.frameInFlight);
	m_graphics_submission.submit_graphics();
}

void rndr_interface::submit_tranfer_cmd() {
	const frame& currentFrame = frames[activeFrameIndex];

	m_tranfer_submission.reset();

	m_tranfer_submission.add_signal(currentFrame.transferFinished);
	m_tranfer_submission.add_cmd(currentFrame.CMDTransfer);

	m_tranfer_submission.submit_transfer();

}

void rndr_interface::present_activeFrame() {
	m_presenter.add_wait(&imageRenderFinished[a_imageIndex], 1);
	m_presenter.present(a_imageIndex);
}

void rndr_interface::advance_frame() noexcept {
	activeFrameIndex = (activeFrameIndex + 1) % bufferedFrame_count;
}
