#pragma once

#include "core/rhi_core.h"
#include "primitive/rhi_primitive.h"
#include "procedure/rhi_procedure.h"
#include "detail/vkexcept.h"

namespace rhi {
	
	namespace core {

		class instance;
		class surface;
		class device;
		class allocator;
	
	}

	namespace primitive {
	
		class commandBuffer;
		class swapchain;

		class buffer;
		class image;
		class sampler;

		class renderPass;
		class framebuffer;

		class semaphore;
		class fence;

		class descriptorLayout;
		class pushConstantRange;
		class pipelineLayout;
		class shader;
		class pipeline;
	
	}

	namespace procedure {
	
		class swapchain_builder;
		class commandPool_creator;
		class commandBuffer_allocator;

		class cmdbuffer_recorder;
		class queue_submission;

		class image_allocator;
		class buffer_allocator;
		class sampler_creator;

		class swapchain_handler;

		class renderPass_builder;
		class pipelineLayout_builder;
		class shader_wrapper;
		class pipeline_builder;
		class framebuffer_creator;
	}

	namespace detail {

	}

}