#pragma once

#include "core_t.hh"
#include "core/rhi_core.h"
#include "primitive/rhi_primitive.h"
#include "procedure/rhi_procedure.h"
#include "detail/vkexcept.h"

namespace rhi {
	
	class instance;
	class surface;
	class device;
	class allocator;

	class pmvCommandBuffer;
	class swapchain;
	class pmvBuffer;
	class pmvImage;
	class pmvSampler;
	class pmvRenderPass;
	class pmvFramebuffer;
	class pmvSemaphorePool;
	class pmvDescriptorSetLayout;
	class pmvPushConstantRange;
	class pmvPipelineLayout;
	class pmvShader;
	class pmvPipeline;

	class pcdCommandPoolCreate;
	class pcdCommandBufferAllocate;
	class pcdCommandBufferRecord;
	class pcdQueueSubmit;
	class pcdWatchdog;
	class pcdImageAllocate;
	class pcdBufferAllocate;
	class pcdSamplerCreate;
	class pcdSwapchainCreate;
	class pcdSwapchainImageAcquire;
	class pcdQueuePresent;
	class pcdRenderPassCreate;
	class pcdPipelineLayoutCreate;
	class pcdShaderModuleCreate;
	class pcdGraphicPipelineCreate;
	class pcdFramebufferCreate;

}