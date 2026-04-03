#pragma once

#include "Signboard/Platform/platform.h"
#include "Signboard/RHI/rhi.h"

class RHIContext;
class rndr_presentation;

struct rndr_interface_Access;

class rndr_interface {
public:
	rndr_interface(const RHIContext& context, const rndr_presentation& presentation);

	~rndr_interface() noexcept;

	void validate_swapchainDependancy();
	void configure_bufferedFrames();

	rhi::pmvCommandBuffer& get_activeFrame_cmd();

	uint32_t acquire_toWriteImage(VkBool32* aquire_optimal) noexcept;
	void submit_activeFrame_cmd();
	void present_activeFrame();

	void advance_frame() noexcept;

private:
	VkResult summon_commandPools();
	void allocate_renderCommandBuffers();

private:
	friend struct rndr_interface_Access;

	const rhi::creDevice& r_device;
	const rhi::pmvSwapchain& r_swapchain;

	rhi::pcdWatchdog m_watchdog;
	rhi::pcdSwapchainHandler m_swapchainHandler;
	rhi::pcdSwapchainPresenter m_presenter;

	rhi::standardCommandPools m_commandPools;
	rhi::pcdQueueSubmission m_graphics_submission;

	uint32_t bufferedFrame_count;

	std::vector<uint32_t> imagesInFlight;
	
	struct frame {
		rhi::pmvFence frameInFlight;
		
		rhi::pmvSemaphore imageAvailable;
		rhi::pmvCommandBuffer cmd;

		frame(const rhi::creDevice& device);
	};
	std::vector<frame> frames;
	uint32_t activeFrameIndex = 0;
	uint32_t a_imageIndex;

	std::vector<rhi::pmvSemaphore> imageRenderFinished;

};
