#pragma once

#include "commandPoolCreate.h"
#include "commandBufferAllocate.h"

#include "commandBufferRecord.h"
#include "renderPassRecord.h"
#include "queueSubmit.h"
#include "watchdog.h"

#include "imageAllocate.h"
#include "bufferAllocate.h"
#include "samplerCreate.h"

#include "swapchainCreate.h"
#include "swapchainImageCreate.h"
#include "queuePresent.h"

#include "descriptorLayoutCreate.h"

#include "renderPassCreate.h"
#include "pipelineLayoutCreate.h"
#include "shaderModuleCreate.h"
#include "pipelineCreate.h"
#include "framebufferCreate.h"

namespace rhi {

	class pcdCommandPoolCreate;
	class pcdCommandBufferAllocate;

	class pcdCommandBufferRecord;
	class pcdRenderPassRecord;
	class pcdQueueSubmit;
	class pcdWatchdog;

	class pcdImageAllocate;
	class pcdBufferAllocate;
	class pcdSamplerCreate;

	class pcdSwapchainCreate;
	class pcdSwapchainImageAllocate;
	class pcdQueuePresent;

	class pcdDescriptorSetLayoutCreate;

	class pcdRenderPassCreate;
	class pcdPipelineLayoutCreate;
	class pcdShaderModuleCreate;
	class pcdGraphicPipelineCreate;
	class pcdFramebufferCreate;

}