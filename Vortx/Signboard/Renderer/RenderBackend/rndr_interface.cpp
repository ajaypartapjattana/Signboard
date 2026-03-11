#include "rndr_interface.h"

#include "rndr_context_Access.h"
#include "rndr_presentation_Access.h"
#include "Signboard/Renderer/Pass/passes_Access.h"

#include <algorithm>
#include <stdexcept>

rndr_interface::rndr_interface(const rndr_context& context, const rndr_presentation& presentation, const passes& passes)
	: 
	r_device(rndr_context_Access::get_device(context)),
	r_surface(rndr_context_Access::get_surface(context)),

	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),

	rm_primaryPass(passes_Access::get_primiaryPass(passes))
{
	summon_commandPools();
	m_graphicsPoolIndex = find_graphicsPool_index();

	configure_bufferedFrames();
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

void rndr_interface::configure_bufferedFrames() {
	create_primaryPass_framebuffers();
	allocate_renderCommandBuffers();
}

VkResult rndr_interface::create_primaryPass_framebuffers() {
	rhi::procedure::framebuffer_creator prcdr{ r_device };

	prcdr.bind_renderpass(rm_primaryPass);

	prcdr.create_swapchainTarget_framebuffer(r_swapchain, &bufferedFrame_count, nullptr);

	m_primaryFramebuffers.resize(bufferedFrame_count);
	return prcdr.create_swapchainTarget_framebuffer(r_swapchain, &bufferedFrame_count, m_primaryFramebuffers.data());
}

void rndr_interface::allocate_renderCommandBuffers() {
	release_renderCommandBuffers();

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

rhi::primitive::commandBuffer& rndr_interface::active_commandBuffer() {
	return m_renderCommandBuffers[m_activeFrameIndex];
}

void rndr_interface::advance_frame() noexcept {
	if (bufferedFrame_count == 0)
		return;

	m_activeFrameIndex = (m_activeFrameIndex + 1) % bufferedFrame_count;
}
