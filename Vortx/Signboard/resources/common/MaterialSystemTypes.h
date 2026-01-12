#pragma once

#include "PipelineSystemTypes.h"
#include "TextureSystemType.h"
#include "SamplerSystemTypes.h"

#include "glm/glm.hpp"

struct MaterialHandle {
	uint32_t index;
	uint32_t generation;

	bool isValid() const {
		return generation != 0;
	}
};
constexpr MaterialHandle INVALID_MATERIAL{ UINT32_MAX, UINT32_MAX };

struct GPUMaterial {
	glm::vec4 baseColor;
	float metallic;
	float roughness;
	uint32_t albedoTex;
	uint32_t normaltex;
};

struct MaterialDesc {
	PipelineHandle pipeline;
	GPUMaterial material;
};