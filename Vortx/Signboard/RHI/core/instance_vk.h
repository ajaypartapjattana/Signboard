#pragma once

#include <vulkan/vulkan.h>
#include "instance.h"

namespace rhi::core {

	struct instance_vkAccess {

		static VkInstance get(const instance& i) noexcept {
			return reinterpret_cast<VkInstance>(i.m_instance);
		}

		static VkAllocationCallbacks* allocator(const instance& i) noexcept {
			return reinterpret_cast<VkAllocationCallbacks*>(i.m_allocator);
		}

	};

}