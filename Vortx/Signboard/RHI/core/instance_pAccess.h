#pragma once

#include "instance.h"

namespace rhi::access {

	struct instance_pAccess {

		static VkInstance get(const rhi::creInstance& i) noexcept {
			return i.m_instance;
		}

	};

}