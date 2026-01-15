#pragma once

#include "Signboard/RHI/vulkan/VulkanCommandBuffer.h"
#include "Signboard/RHI/vulkan/VulkanImage.h"
#include "Signboard/RHI/vulkan/VulkanSemaphore.h"
#include "Signboard/RHI/vulkan/VulkanRenderPass.h"

#include "Signboard/RHI/vulkan/VulkanBuffer.h"

#include "Signboard/RHI/VulkanRHI.h"
#include "Signboard/Resources/Resource.h"
#include "Signboard/Scene/Scene.h"

#include <array>
#include <vector>
#include <memory>

static constexpr uint32_t FRAMES_IN_FLIGHT = 2;

struct RenderQueue {
	std::vector<ObjectHandle> drawList;
};

class VulkanDevice;

class VulkanFrameBuffer;
class VulkanRenderPass;

class Renderer {
public:
	explicit Renderer(RHIView& HInterface, ResourceView resources, SceneView scene);
	~Renderer();

	bool prepareFrame();
	void configureDraws();
	void renderFrame();
	void endFrame();

	void resize();

private:
	VulkanBuffer createIndirectDrawBuffer();
	VulkanRenderPass createRenderPass();

private:
	RHIView HInterface;
	ResourceView resources;
	SceneView scene;

	VulkanBuffer IndirectDrawBuffer;

	VulkanRenderPass renderPass;

	uint32_t currentFrameIndex;
	uint32_t acquiredImageIndex;

	bool graphDirty = true;

	struct Frame {
		VulkanCommandBuffer cmd;
		VulkanSemaphore imageAvailable;
		VulkanSemaphore renderFinished;
	};

	std::vector<Frame> frames;

}