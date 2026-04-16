#pragma once

#include "core/rhi_core.h"
#include "primitive/rhi_primitive.h"
#include "procedure/rhi_procedure.h"
#include "detail/vkexcept.h"

namespace rhi {
	
	class creInstance;
	class creSurface;
	class creDevice;
	class creAllocator;

	class pmvCommandBuffer;
	class pmvSwapchain;
	class pmvBuffer;
	class pmvImage;
	class pmvSampler;
	class pmvRenderPass;
	class pmvFramebuffer;
	class pmvSemaphore;
	class pmvFence;
	class pmvDescriptorLayout;
	class pmvPushConstantRange;
	class pmvPipelineLayout;
	class pmvShader;
	class pmvPipeline;

	class pcdCommandPoolCreator;
	class pcdCommandBufferAllocator;
	class pcdCommandBufferRecorder;
	class pcdQueueSubmission;
	class pcdWatchdog;
	class pcdImageAllocator;
	class pcdBufferAllocator;
	class pcdSamplerCreator;
	class pcdSwapchainBuilder;
	class pcdSwapchainHandler;
	class pcdSwapchainPresenter;
	class pcdRenderPassBuilder;
	class pcdPipelineLayoutCreate;
	class pcdShaderWrapper;
	class pcdPipelineBuilder;
	class pcdFramebufferCreator;

}