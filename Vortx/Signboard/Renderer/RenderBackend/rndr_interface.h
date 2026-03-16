#pragma once

#include "Signboard/Platform/platform.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;
class rndr_presentation;

struct rndr_interface_Access;

class rndr_interface {
public:
	rndr_interface(const rndr_context& context, const rndr_presentation& presentation);

	~rndr_interface() noexcept;

	void configure_bufferedFrames();

	rhi::primitive::commandBuffer& get_activeFrame_cmd();

	uint32_t acquire_toWriteImage() noexcept;
	void submit_activeFrame_cmd();
	void present_activeFrame(uint32_t toPresent_Image);

	void advance_frame() noexcept;

private:
	VkResult summon_commandPools();
	void allocate_renderCommandBuffers();

private:
	friend struct rndr_interface_Access;

	const rhi::core::device& r_device;
	const rhi::primitive::swapchain& r_swapchain;

	rhi::procedure::_watchdog m_watchdog;
	rhi::procedure::swapchain_handler m_swapchainHandler;
	rhi::procedure::swapchain_presenter m_presenter;

	std_commandPools m_commandPools;
	rhi::procedure::queue_submission m_graphics_submission;

	uint32_t bufferedFrame_count = 2;

	struct frame {
		rhi::primitive::semaphore image_available;
		rhi::primitive::semaphore render_finished;
		rhi::primitive::fence in_flight;
		
		rhi::primitive::commandBuffer cmd;

		frame(const rhi::core::device& device);
	};
	std::vector<frame> frames;

	uint32_t activeFrameIndex = 0;

};
