#pragma once

#include "descriptorLayout.h"

namespace rhi::primitive {

	struct descriptorLayout_vkAccess {

		static VkDescriptorSetLayout get(const descriptorLayout& dl) noexcept {
			return dl.m_setLayout;
		}

	};

}