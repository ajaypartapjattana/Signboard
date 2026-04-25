#pragma once

#include "commandPoolCreator.h"
#include "commandBufferAllocator.h"

#include "commandBufferRecorder.h"
#include "queueSubmission.h"
#include "watchdog.h"

#include "imageAllocator.h"
#include "bufferAllocator.h"
#include "samplerCreator.h"

#include "swapchainBuilder.h"
#include "swapchainImageCreate.h"
#include "swapchainHandler.h"
#include "swapchainPresenter.h"

#include "descriptorLayoutCreate.h"

#include "renderPassBuilder.h"
#include "pipelineLayoutBuilder.h"
#include "shaderWrapper.h"
#include "pipelineBuilder.h"
#include "framebufferCreator.h"

namespace rhi {

	class pcdCommandPoolCreator;
	class pcdCommandBufferAllocator;

	class pcdCommandBufferRecorder;
	class pcdQueueSubmission;
	class pcdWatchdog;

	class pcdImageAllocate;
	class pcdBufferAllocate;
	class pcdSamplerCreator;

	class pcdSwapchainCreate;
	class pcdSwapchainImageAllocate;
	class pcdSwapchainHandler;
	class pcdSwapchainPresenter;

	class pcdDescriptorSetLayoutCreate;

	class pcdRenderPassCreate;
	class pcdPipelineLayoutCreate;
	class pcdShaderModuleCreate;
	class pcdPipelineBuilder;
	class pcdFramebufferCreate;

}