#pragma once

#include "commandPool_creator.h"
#include "commandBuffer_allocator.h"

#include "cmdbuffer_recorder.h"
#include "queue_submission.h"
#include "_watchdog.h"

#include "image_allocator.h"
#include "buffer_allocator.h"
#include "sampler_creator.h"

#include "swapchain_builder.h"
#include "swapchain_handler.h"
#include "swapchain_presenter.h"

#include "renderPass_builder.h"
#include "pipelineLayout_builder.h"
#include "shader_wrapper.h"
#include "pipeline_builder.h"
#include "framebuffer_creator.h"

namespace rhi::procedure {

	class commandPool_creator;
	class commandBuffer_allocator;

	class cmdbuffer_recorder;
	class queue_submission;
	class _watchdog;

	class image_allocator;
	class buffer_allocator;
	class sampler_creator;

	class swapchain_builder;
	class swapchain_handler;
	class swapchain_presenter;

	class renderPass_builder;
	class pipelineLayout_builder;
	class shader_wrapper;
	class pipeline_builder;
	class framebuffer_creator;

}