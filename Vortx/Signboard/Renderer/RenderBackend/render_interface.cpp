#include "render_interface.h"

#include <algorithm>
#include <stdexcept>

render_interface::render_interface(const platform::primitive::display_window& a_window, uint32_t bufferedFrame_count) 
	: m_bufferedFrameCount(std::max(2u, bufferedFrame_count)),

	m_instance(setup_instance()),
	m_surface(setup_surface(a_window)),

	m_device(setup_device()),

	m_swapchain(setup_swapchain()),
	m_allocator(setup_allocator())
{
	rhi::procedure::commandPool_creator l_creator{ m_device };

	const auto& poolRequirements = l_creator.get_poolRequirements();
	const uint32_t poolCount = static_cast<uint32_t>(poolRequirements.size());

	m_commandPools.resize(poolCount);
	if (poolCount > 0) {
		VkResult result = l_creator.create(m_commandPools.data(), poolCount);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: commandPool_creation!");
	}

	m_commandPoolBindings.reserve(poolRequirements.size());
	for (uint32_t i = 0; i < poolCount; ++i) {
		m_commandPoolBindings.push_back({
			i,
			poolRequirements[i].family,
			poolRequirements[i].capabilities,
			poolRequirements[i].present_supported
		});
	}

	m_graphicsPoolIndex = find_graphicsPool_index();
	allocate_renderCommandBuffers(m_bufferedFrameCount);
}

rhi::core::instance render_interface::setup_instance() {
	rhi::procedure::instance_builder l_builder;

	uint32_t ext_count = 0;
	const char** exts = platform::core::context::native_extensions(ext_count);

	for (uint32_t i = 0; i < ext_count; ++i) {
		l_builder.addExtension(exts[i]);
	}

#ifndef NDEBUG
	l_builder.enableValidationLayer();
	l_builder.addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return l_builder.build();
}

rhi::core::surface render_interface::setup_surface(const platform::primitive::display_window& window) {
	rhi::procedure::surface_creator l_creator{ m_instance };

	return l_creator.create_surface(window);
}

rhi::core::device render_interface::setup_device() {
	rhi::procedure::device_builder l_builder{ m_instance };

	l_builder.require_graphicsQueue();
	l_builder.require_transferQueue();
	l_builder.require_presentQueue(m_surface);

	l_builder.enable_samplerAnisotropy();

	return l_builder.build();
}

rhi::core::swapchain render_interface::setup_swapchain() {
	rhi::procedure::swapchain_builder l_builder{ m_device, m_surface };

	l_builder.prefer_format_srgb();

	l_builder.set_imageCount(m_bufferedFrameCount);

	return l_builder.build();
}

rhi::core::allocator render_interface::setup_allocator() {
	rhi::procedure::allocator_creator l_creator{ m_instance, m_device };

	return l_creator.create();
}

uint32_t render_interface::find_graphicsPool_index() const noexcept {
	auto it = std::find_if(
		m_commandPoolBindings.begin(),
		m_commandPoolBindings.end(),
		[](const commandPool_binding& binding) {
			return (binding.capabilities & VK_QUEUE_GRAPHICS_BIT) != 0;
		}
	);

	if (it == m_commandPoolBindings.end())
		return 0;

	return it->poolIndex;
}

void render_interface::allocate_renderCommandBuffers(uint32_t bufferedFrame_count) {
	release_renderCommandBuffers();

	if (bufferedFrame_count == 0)
		return;

	if (m_commandPools.empty())
		return;

	m_renderCommandBuffers.resize(bufferedFrame_count);

	rhi::procedure::commandBuffer_allocator l_allocator{ m_device };
	l_allocator.allocate(m_commandPools[m_graphicsPoolIndex], m_renderCommandBuffers.data(), bufferedFrame_count);
}

void render_interface::release_renderCommandBuffers() noexcept {
	if (m_renderCommandBuffers.empty())
		return;

	if (m_commandPools.empty())
		return;

	// --- incomplete here! [add free/release capability to the commandBuffer_allocator before proceeding->]

}

void render_interface::set_bufferedFrame_count(uint32_t bufferedFrame_count) {
	uint32_t clampedFrames = std::max(2u, bufferedFrame_count);
	if (clampedFrames == m_bufferedFrameCount)
		return;

	m_bufferedFrameCount = clampedFrames;

	rhi::procedure::swapchain_builder l_builder{ m_device, m_surface };

	l_builder.prefer_format_srgb();
	l_builder.set_imageCount(m_bufferedFrameCount);
	l_builder.recycle_swapchain(m_swapchain);
	m_swapchain = l_builder.build();

	allocate_renderCommandBuffers(bufferedFrame_count);
}

uint32_t render_interface::get_bufferedFrame_count() const noexcept {
	return m_bufferedFrameCount;
}

rhi::primitive::commandBuffer& render_interface::active_commandBuffer() {
	return m_renderCommandBuffers[m_activeFrameIndex];
}

void render_interface::advance_frame() noexcept {
	if (m_bufferedFrameCount == 0)
		return;

	m_activeFrameIndex = (m_activeFrameIndex + 1) % m_bufferedFrameCount;
}
