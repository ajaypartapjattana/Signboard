#pragma once

#include "Signboard/Platform/platform.h"
#include "Signboard/RHI/rhi.h"

class render_interface {
public:
	render_interface(const platform::primitive::display_window& a_window, uint32_t bufferedFrame_count = 2);

	void set_bufferedFrame_count(uint32_t bufferedFrame_count);
	uint32_t get_bufferedFrame_count() const noexcept;

	rhi::primitive::commandBuffer& active_commandBuffer();
	void advance_frame() noexcept;

private:
	rhi::core::instance setup_instance();
	rhi::core::surface setup_surface(const platform::primitive::display_window& window);

	rhi::core::device setup_device();

	rhi::core::swapchain setup_swapchain();
	rhi::core::allocator setup_allocator();

private:
	uint32_t find_graphicsPool_index() const noexcept;

	void allocate_renderCommandBuffers(uint32_t buffered_frames);
	void release_renderCommandBuffers() noexcept;

private:
	rhi::core::instance m_instance;
	rhi::core::surface m_surface;

	rhi::core::device m_device;

	rhi::core::swapchain m_swapchain;
	rhi::core::allocator m_allocator;

private:
	uint32_t m_bufferedFrameCount = 2;
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
