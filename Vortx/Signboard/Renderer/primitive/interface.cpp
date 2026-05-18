#include "Signboard/Renderer/Internal/rndr_pAccess.h"

#include <algorithm>
#include <stdexcept>

namespace rndr {

	rndr_interface::rndr_interface(const creContext& context, const crePresentation& presentation)
		:
		r_swapchain(_pAccess::swapchain(presentation)),

		m_presenter(_pAccess::device(context)),

		m_queueSubmit()
	{
		m_presenter.target_swapchains({ &r_swapchain, 1 });

		

		configure_bufferedFrames(presentation.availableImageCount());
	}

	rndr_interface::~rndr_interface() noexcept {
	}



	VkResult rndr_interface::summon_commandPools() {
		
	}

	void rndr_interface::validate_swapchainDependancy() {
		m_presenter.target_swapchains({ &r_swapchain, 1 });
	}

	void rndr_interface::pushRenderJob(const frame& frame) {
		m_queueSubmit.target_commandBuffers({ &frame.CMDGraphics, 1 });

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		m_queueSubmit.target_waitSemaphores({ &frame.imageAvailable, 1 }, { &waitStage, 1 });

		if (waitTransfer)
			m_queueSubmit.target_waitSemaphores(currentFrame.transferFinished, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);

		m_queueSubmit.target_signalSemaphores(imageRenderFinished[a_imageIndex]);

		m_queueSubmit.submit( ,frame.frameInFlight);
	}

	void rndr_interface::pushUploadJob(const frame& frame) {
		m_queueSubmit.target_signalSemaphores({ &frame.transferFinished, 1 });
		m_queueSubmit.target_commandBuffers({ &frame.CMDTransfer, 1 });

		m_queueSubmit.submit();

	}

	void rndr_interface::pushPresentJob() {
		m_presenter.target_waitSemaphores(&imageRenderFinished[a_imageIndex], 1);
		m_presenter.present(a_imageIndex);
	}

}
