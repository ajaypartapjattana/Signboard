#pragma once

#include "descriptorLayout.h"

namespace rhi::access {

	struct descriptorLayout_pAccess {

		static VkDescriptorSetLayout get(const rhi::pmvDescriptorLayout& dl) noexcept {
			return dl.m_setLayout;
		}

	};

}