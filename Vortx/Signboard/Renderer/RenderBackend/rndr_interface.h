#pragma once

#include "Signboard/Platform/platform.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;
class rndr_presentation;
class passes;

struct rndr_interface_Access;

class rndr_interface {
public:
	rndr_interface(const rndr_context& context, const rndr_presentation& presentation, const passes& passes);

	void set_bufferedFrame_count(uint32_t bufferedFrame_count);
	uint32_t get_bufferedFrame_count() const noexcept;

	rhi::primitive::commandBuffer& active_commandBuffer();
	void advance_frame() noexcept;

private:
	VkResult create_swapchainTargetFB();

	uint32_t find_graphicsPool_index() const noexcept;

	void allocate_renderCommandBuffers(uint32_t buffered_frames);
	void release_renderCommandBuffers() noexcept;

private:
	friend struct rndr_interface_Access;

	const rhi::core::device& r_device;
	const rhi::core::surface& r_surface;
	const rhi::primitive::swapchain& r_swapchain;

	const rhi::primitive::renderPass& rm_primaryPass;
	std::vector<rhi::primitive::framebuffer> m_scBuffers;

	uint32_t bufferedFrame_count = 2;
	uint32_t m_activeFrameIndex = 0;

	struct commandPool_binding {
		uint32_t poolIndex;
		uint32_t queueFamily;
		VkQueueFlags capabilities;
		bool presentSupported;
	};

	std::vector<rhi::core::commandPool> m_commandPools;
	std::vector<commandPool_binding> m_commandPoolBindings;

	uint32_t m_graphicsPoolIndex = 0;
	std::vector<rhi::primitive::commandBuffer> m_renderCommandBuffers;

};
