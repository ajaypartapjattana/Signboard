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
	}

	void rndr_interface::validate_swapchainDependancy() {
		m_presenter.target_swapchains({ &r_swapchain, 1 });
	}

	void rndr_interface::pushRenderJob(const frame& frame) {
		m_queueSubmit.target_commandBuffers({ &frame.CMDGraphics, 1 });

		m_queueSubmit.target_waitSemaphores(frame.waitSemaphores, frame.waitStages);
		m_queueSubmit.target_signalSemaphores({ &frame.renderFinished, 1 });

		m_queueSubmit.submit( ,frame.inFlight);
	}

	void rndr_interface::pushUploadJob(frame& frame) {
		m_queueSubmit.target_signalSemaphores({ &frame.transferFinished, 1 });
		m_queueSubmit.target_commandBuffers({ &frame.CMDTransfer, 1 });

		m_queueSubmit.submit();

		frame.waitSemaphores.push_back(frame.transferFinished);
	}

	// [create a syncPool managed by the submission interface, which just pushes 'to-wait' semaphores into the frame container]

	void rndr_interface::pushPresentJob(const frame& frame) noexcept {
		m_presenter.target_waitSemaphores({ &frame.renderFinished, 1 });
		m_presenter.present(frame.assignedImage);
	}

}
