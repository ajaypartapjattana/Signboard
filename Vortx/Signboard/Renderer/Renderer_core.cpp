#include "Renderer.h"

#include "DescriptorSchema.h"

Renderer::Renderer(RHIView& HInterface, ResourceView resources, SceneView scene)
	: HInterface(HInterface), resources(resources), scene(scene), IndirectDrawBuffer(createIndirectDrawBuffer()), renderPass(createRenderPass())
{
	frames.resize(FRAMES_IN_FLIGHT);
}

VulkanBuffer Renderer::createIndirectDrawBuffer() {
	BufferDesc drawB;
	drawB.size = 20 * DESCRIPTOR_SCHEMA::OBJECT_STATE::MAX_OBJECT_COUNT;
	drawB.memoryFlags.set(MemoryProperty::HostCoherent, MemoryProperty::HostVisible);
	drawB.usageFlags = BufferUsage::IndirectDraw;

	return VulkanBuffer(HInterface.device, drawB);
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

bool Renderer::prepareFrame() {
	Frame& currentFrame = frames[currentFrameIndex];

	SwapchainImageAcquire acquire =  HInterface.swapchain.accquireNextImage(currentFrame.imageAvailable);

	if (acquire.result == SwapchianAcquireResult::OutOfDate || acquire.result == SwapchianAcquireResult::SurfaceLost) {
		resize();
		return false;
	}

	acquiredImageIndex = acquire.imageIndex;

	currentFrame.cmd.begin();

	return true;
}

void Renderer::configureDraws() {

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