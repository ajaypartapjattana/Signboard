#pragma once

#include "Signboard/Platform/platform.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;
class rndr_presentation;

struct rndr_interface_Access;

class rndr_interface {
public:
	rndr_interface(const rndr_context& context, const rndr_presentation& presentation);

	void configure_bufferedFrames();

	rhi::primitive::commandBuffer& get_activeFrame_cmd();
	void advance_frame() noexcept;

	uint32_t acquire_toWriteImage() const noexcept;
	void submit_activeFrame_cmd();

private:
	VkResult summon_commandPools();
	void acquire_graphicsPool_index() noexcept;

	void allocate_renderCommandBuffers();
	void release_renderCommandBuffers() noexcept;

private:
	friend struct rndr_interface_Access;

	const rhi::core::device& r_device;
	const rhi::primitive::swapchain& r_swapchain;

	rhi::procedure::swapchain_handler m_swapchainHandler;

	struct commandPool_binding {
		uint32_t poolIndex;
		uint32_t queueFamily;
		VkQueueFlags capabilities;
		bool presentSupported;
	};
	std_commandPools m_commandPools;
	uint32_t m_graphicsPoolIndex = 0;

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
