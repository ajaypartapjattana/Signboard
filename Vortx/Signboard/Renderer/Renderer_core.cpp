#include "Renderer.h"

Renderer::Renderer(RHIView& HInterface, ResourceView resources, SceneView& scene)
	: HInterface(HInterface), resources(resources), scene(scene), renderPass(createRenderPass())
{
	frames.resize(FRAMES_IN_FLIGHT);
}

VulkanRenderPass Renderer::createRenderPass() {
	AttachmentDesc colorAttachment{};
	colorAttachment.format = ImageFormat::RGBA8;
	colorAttachment.load = LoadOp::Clear;
	colorAttachment.store = StoreOp::Store;

	AttachmentDesc depthAttachment{};
	depthAttachment.format = ImageFormat::Depth24Stencil8;
	depthAttachment.load = LoadOp::DontCare;
	depthAttachment.store = StoreOp::DontCare;

	RenderPassDesc pass{};
	pass.colorAttachments = { colorAttachment };
	pass.hasDepth = true;
	pass.depthAttachment = depthAttachment;

	return VulkanRenderPass{HInterface.device, pass};
}

void Renderer::beginFrame() {
	Frame& currentFrame = frames[currentFrameIndex];

	SwapchainImageAcquire acquire =  HInterface.swapchain.accquireNextImage(currentFrame.imageAvailable);

	if (acquire.result == SwapchianAcquireResult::OutOfDate || acquire.result == SwapchianAcquireResult::SurfaceLost) {
		resize();
		return;
	}

	acquiredImageIndex = acquire.imageIndex;

	currentFrame.cmd.begin();
}

void Renderer::renderFrame() {
	
}

void Renderer::endFrame() {
	Frame& currentFrame = frames[currentFrameIndex];

	currentFrame.cmd.end();

	currentFrame.cmd.submit(currentFrame.imageAvailable, currentFrame.renderFinished);
	HInterface.swapchain.present(acquiredImageIndex, currentFrame.renderFinished);

	currentFrameIndex = (currentFrameIndex + 1) % FRAMES_IN_FLIGHT;
}