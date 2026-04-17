#pragma once

#include "descriptorLayout.h"

namespace rhi::access {

	struct descriptorSetLayout_pAccess {

		static VkDescriptorSetLayout extract(const rhi::pmvDescriptorSetLayout& dl) noexcept {
			return dl.m_setLayout;
		}

	};

}