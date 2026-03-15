#include "rndr_interface.h"

#include "rndr_context_Access.h"
#include "rndr_presentation_Access.h"

#include <algorithm>
#include <stdexcept>

rndr_interface::rndr_interface(const rndr_context& context, const rndr_presentation& presentation)
	: 
	r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),

	m_swapchainHandler(r_device)
{
	summon_commandPools();
	acquire_graphicsPool_index();

	configure_bufferedFrames();
}

rndr_interface::frame::frame(const rhi::core::device& device)
	: 
	image_available(device),
	render_finished(device),
	in_flight(device, true)
{

}

uint32_t rndr_interface::acquire_toWriteImage() const noexcept {
	uint32_t a_imageIndex;
	m_swapchainHandler.acquire_freeSwapchainImage(&frames[activeFrameIndex].image_available, a_imageIndex);

	return a_imageIndex;
}

VkResult rndr_interface::summon_commandPools() {
	rhi::procedure::commandPool_creator prcdr{ r_device };

	const auto& poolRequirements = prcdr.get_poolRequirements();
	const uint32_t poolCount = static_cast<uint32_t>(poolRequirements.size());

	m_commandPools.resize(poolCount);
	VkResult result = prcdr.create(m_commandPools.data(), poolCount);
	if (result != VK_SUCCESS)
		return result;

	m_commandPoolBindings.reserve(poolRequirements.size());
	for (uint32_t i = 0; i < poolCount; ++i) {
		m_commandPoolBindings.push_back({
			i,
			poolRequirements[i].family,
			poolRequirements[i].capabilities,
			poolRequirements[i].present_supported
			});
	}

	return VK_SUCCESS;
}

void rndr_interface::acquire_graphicsPool_index() noexcept {
	auto it = std::find_if(
		m_commandPoolBindings.begin(),
		m_commandPoolBindings.end(),
		[](const commandPool_binding& binding) {
			return binding.capabilities & VK_QUEUE_GRAPHICS_BIT;
		}
	);

	m_graphicsPoolIndex = (it == m_commandPoolBindings.end()) ? UINT32_MAX : it->poolIndex;
}

void rndr_interface::configure_bufferedFrames() {
	bufferedFrame_count = r_swapchain.native_imageCount();

	frames.reserve(bufferedFrame_count);
	for (uint32_t i = 0; i < bufferedFrame_count; ++i)
		frames.emplace_back(r_device);

	allocate_renderCommandBuffers();
}

void rndr_interface::allocate_renderCommandBuffers() {
	release_renderCommandBuffers();

	if (m_graphicsPoolIndex == UINT32_MAX)
		return;

	rhi::procedure::commandBuffer_allocator l_allocator{ r_device };

	for (uint32_t i = 0; i < bufferedFrame_count; i++) {
		l_allocator.allocate(m_commandPools[m_graphicsPoolIndex], &frames[i].cmd, 1);
	}
}

void rndr_interface::release_renderCommandBuffers() noexcept {
	if (m_commandPools.empty())
		return;

	// --- incomplete here! [add free/release capability to the commandBuffer_allocator before proceeding->]

}

rhi::primitive::commandBuffer& rndr_interface::activeFrame_cmd() {
	return frames[activeFrameIndex].cmd;
}

void rndr_interface::advance_frame() noexcept {
	activeFrameIndex = (activeFrameIndex + 1) % bufferedFrame_count;
}
