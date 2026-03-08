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
		class commandPool;
	
	}

	namespace primitive {
	
		class commandBuffer;
		class swapchain;

		class buffer;
		class image;
		class sampler;
	
	}

	namespace procedure {
	
		class swapchain_builder;
		class commandPool_creator;

		class commandBuffer_allocator;
		class image_allocator;
		class buffer_allocator;
		class sampler_creator;
	
	}

	namespace detail {

	}

}