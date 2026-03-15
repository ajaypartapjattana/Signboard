#include "rndr_framedraw.h"

#include "rndr_context_Access.h"
#include "rndr_presentation_Access.h"


#include "Signboard/Renderer/Pass/passes_Access.h"

rndr_framedraw::rndr_framedraw(const rndr_context& context, const rndr_presentation& presentation, const rndr_method& methods)
	:
	r_device(rndr_context_Access::get_device(context)),
	r_swapchain(rndr_presentation_Access::get_swapchain(presentation)),

	r_primaryPass(passes_Access::get_primiaryPass(passes))
{
	create_primaryPass_framebuffers();
}

VkResult rndr_framedraw::create_primaryPass_framebuffers() {
	rhi::procedure::framebuffer_creator prcdr{ r_device };

	prcdr.bind_renderpass(r_primaryPass);

	prcdr.create_swapchainTarget_framebuffer(r_swapchain, &bufferedFrame_count, nullptr);

	m_primaryFramebuffers.resize(bufferedFrame_count);
	return prcdr.create_swapchainTarget_framebuffer(r_swapchain, &bufferedFrame_count, m_primaryFramebuffers.data());
}

void rndr_framedraw::drawFrame(const uint32_t target_index, const rhi::primitive::commandBuffer& target_cmd) {
	rhi::procedure::cmdbuffer_recorder prcdr{ target_cmd };

	prcdr.begin_renderTarget(m_primaryFramebuffers[target_index]);
	prcdr.bind_pipeline();
	prcdr.draw();
	prcdr.end_renderTarget();
	prcdr.end_recording();

}