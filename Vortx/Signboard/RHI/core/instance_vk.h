#pragma once

#include "instance.h"

namespace rhi::core {

	struct instance_vkAccess {

		static VkInstance get(const instance& i) noexcept {
			return i.m_instance;
		}

		static VkAllocationCallbacks* allocator(const instance& i) noexcept {
			return i.m_allocator;
		}

	};

}