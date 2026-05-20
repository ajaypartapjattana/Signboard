#include "Signboard/Renderer/Internal/rndr_pAccess.h"

#include <algorithm>
#include <stdexcept>

namespace rndr {

	rndr_interface::rndr_interface(const creContext& context, const crePresentation& presentation)
		:
		r_device(_pAccess::device(context)),
		r_swapchain(_pAccess::swapchain(presentation)),

		m_presenter(_pAccess::device(context)),
		m_queueSubmit()
	{
		m_presenter.target_swapchains({ &r_swapchain, 1 });
	}

	void rndr_interface::validate_swapchainDependancy() {
		m_presenter.target_swapchains({ &r_swapchain, 1 });
	}

	void acquireImageJob(frame& frame) {

	}

	void rndr_interface::pushRenderJob(frame& frame) {
		m_queueSubmit.target_commandBuffers({ &frame.CMDGraphics, 1 });

		m_queueSubmit.waitSemaphores(frame.waitSemaphores, frame.submissionWaitStages);

		VkSemaphore signal = frame.semaphorePool.grant();
		m_queueSubmit.signalSemaphores({ &signal, 1 });

		m_queueSubmit.submit(r_device.graphics(), &frame.inFlight);

		frame.waitSemaphores.clear();
		frame.waitSemaphores.push_back(signal);
	}

	void rndr_interface::pushUploadJob(frame& frame) {
		VkSemaphore signal = frame.semaphorePool.grant();
		m_queueSubmit.signalSemaphores({ &signal, 1 });
		m_queueSubmit.target_commandBuffers({ &frame.CMDTransfer, 1 });

		m_queueSubmit.submit(r_device.transfer(), nullptr);

		frame.waitSemaphores.push_back(signal);
	}

	void rndr_interface::pushPresentJob(const frame& frame) noexcept {
		m_presenter.target_waitSemaphores(frame.waitSemaphores);
		m_presenter.present(frame.assignedImage);
	}

}
