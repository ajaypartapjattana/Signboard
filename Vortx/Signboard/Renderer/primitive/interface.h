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

		~rndr_interface() noexcept;

		void validate_swapchainDependancy();

		void pushRenderJob(const frame& frame);
		void pushUploadJob(const frame& frame);
		void pushPresentJob();

		void advance_frame() noexcept;

	private:
		VkResult summon_commandPools();
		void allocate_command_buffers();

	private:
		friend struct _pAccess;

		const rhi::creSwapchain& r_swapchain;

		rhi::pcdQueuePresent m_presenter;
		rhi::pcdQueueSubmit m_queueSubmit;

		uint32_t bufferedFrame_count;

		 // [move frames owner-ship to renderer rather than the interface.
		


	};

}
