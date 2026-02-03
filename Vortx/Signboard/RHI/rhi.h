#pragma once

#include "core/rhi_core.h"
#include "primitive/rhi_primitive.h"
#include "procedure/rhi_procedure.h"

namespace rhi {
	
	namespace core {
		class instance;
		class surface;
		class device;
		class swapchain;
		class allocator;
		class commandPool;
	}

	namespace primitive {
		class buffer;
		class image;
		class sampler;
	}

	namespace procedure {
		class instance_builder;
		class surface_creator;
		class device_builder;
		class swapchain_builder;
		class allocator_creator;
		class commandPool_creator;

		class image_allocator;
		class buffer_allocator;
		class sampler_creator;
	}

	namespace detail {

	}

}