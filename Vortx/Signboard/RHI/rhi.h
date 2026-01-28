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
	}

	namespace primitive {
		class buffer;
		class image;
	}

	namespace procedure {
		class instance_builder;
		class surface_creator;
		class device_builder;
		class swapchain_builder;
		class allocator_creator;

		class image_allocator;
	}

	namespace detail {

	}

}