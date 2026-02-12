#pragma once

#include "shader.h"

namespace rhi::primitive {

	struct shader_vkAccess {

		static VkShaderModule get(const shader& s) noexcept {
			return s.m_shader;
		}
		
	};

}