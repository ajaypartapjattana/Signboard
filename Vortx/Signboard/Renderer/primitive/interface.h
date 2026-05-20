#pragma once

#include "Signboard/Renderer/Internal/rndr_pInterface.h"

#include "Signboard/Platform/platform.h"
#include "Signboard/RHI/rhi.h"

namespace rndr {

	class creContext;
	class crePresentation;

	struct _pAccess;

	class rndr_interface {
	public:
		rndr_interface(const creContext& context, const crePresentation& presentation);

		void validate_swapchainDependancy();

		void acquireImageJob(frame& frame);

		void pushRenderJob(frame& frame);
		void pushUploadJob(frame& frame);
		void pushPresentJob(const frame& frame) noexcept;

	private:
		friend struct _pAccess;

		const rhi::creDevice& r_device;
		const rhi::creSwapchain& r_swapchain;

		rhi::pcdQueuePresent m_presenter;
		rhi::pcdQueueSubmit m_queueSubmit;

	};

}
