#include "rndr_interface.h"

#include "rndr_context_Access.h"

#include <algorithm>
#include <stdexcept>

rndr_interface::rndr_interface(const rndr_context& context, uint32_t bufferedFrame_count)
	: 
	r_device(rndr_context_Access::get_device(context)),
	r_surface(rndr_context_Access::get_surface(context)),

	m_bufferedFrameCount(std::max(2u, bufferedFrame_count)),
	m_swapchain(setup_swapchain())
{
	rhi::procedure::commandPool_creator l_creator{ r_device };

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



rhi::core::swapchain rndr_interface::setup_swapchain() {
	rhi::procedure::swapchain_builder l_builder{ r_device, r_surface };

	l_builder.prefer_format_srgb();

	l_builder.set_imageCount(m_bufferedFrameCount);

	return l_builder.build();
}

uint32_t rndr_interface::find_graphicsPool_index() const noexcept {
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

void rndr_interface::allocate_renderCommandBuffers(uint32_t bufferedFrame_count) {
	release_renderCommandBuffers();

	if (bufferedFrame_count == 0)
		return;

	if (m_commandPools.empty())
		return;

	m_renderCommandBuffers.resize(bufferedFrame_count);

	rhi::procedure::commandBuffer_allocator l_allocator{ r_device };
	l_allocator.allocate(m_commandPools[m_graphicsPoolIndex], m_renderCommandBuffers.data(), bufferedFrame_count);
}

void rndr_interface::release_renderCommandBuffers() noexcept {
	if (m_renderCommandBuffers.empty())
		return;

	if (m_commandPools.empty())
		return;

	// --- incomplete here! [add free/release capability to the commandBuffer_allocator before proceeding->]

}

void rndr_interface::set_bufferedFrame_count(uint32_t bufferedFrame_count) {
	uint32_t clampedFrames = std::max(2u, bufferedFrame_count);
	if (clampedFrames == m_bufferedFrameCount)
		return;

	m_bufferedFrameCount = clampedFrames;

	rhi::procedure::swapchain_builder l_builder{ r_device, r_surface };

	l_builder.prefer_format_srgb();
	l_builder.set_imageCount(m_bufferedFrameCount);
	l_builder.recycle_swapchain(m_swapchain);
	m_swapchain = l_builder.build();

	allocate_renderCommandBuffers(bufferedFrame_count);
}

uint32_t rndr_interface::get_bufferedFrame_count() const noexcept {
	return m_bufferedFrameCount;
}

rhi::primitive::commandBuffer& rndr_interface::active_commandBuffer() {
	return m_renderCommandBuffers[m_activeFrameIndex];
}

void rndr_interface::advance_frame() noexcept {
	if (m_bufferedFrameCount == 0)
		return;

	m_activeFrameIndex = (m_activeFrameIndex + 1) % m_bufferedFrameCount;
}
