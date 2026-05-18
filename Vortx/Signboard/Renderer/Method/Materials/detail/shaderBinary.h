#pragma once

#include <vector>

#include "Signboard/RHI/rhi.h"

struct shaderBinary {
	VkShaderStageFlagBits stage;
	std::vector<uint32_t> data;
};