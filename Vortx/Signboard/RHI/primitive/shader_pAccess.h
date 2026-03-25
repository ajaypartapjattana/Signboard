#pragma once

#include "shader.h"

namespace rhi::access {

	struct shader_pAccess {

		static VkShaderModule get(const rhi::pmvShader& s) noexcept {
			return s.m_shader;
		}
		
	};

}