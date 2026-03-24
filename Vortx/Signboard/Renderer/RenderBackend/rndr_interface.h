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

	void validate_swapchainDependancy();
	void configure_bufferedFrames();

	rhi::primitive::commandBuffer& get_activeFrame_cmd();

	uint32_t acquire_toWriteImage(VkBool32* aquire_optimal) noexcept;
	void submit_activeFrame_cmd();
	void present_activeFrame();

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

	uint32_t bufferedFrame_count;

	std::vector<uint32_t> imagesInFlight;
	
	struct frame {
		rhi::primitive::fence frameInFlight;
		
		rhi::primitive::semaphore imageAvailable;
		rhi::primitive::commandBuffer cmd;

		frame(const rhi::core::device& device);
	};
	std::vector<frame> frames;
	uint32_t activeFrameIndex = 0;
	uint32_t a_imageIndex;

	std::vector<rhi::primitive::semaphore> imageRenderFinished;

};
