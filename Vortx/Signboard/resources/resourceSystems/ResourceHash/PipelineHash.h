#pragma once

#include "Signboard/RHI/common/PipelineTypes.h"
#include "HashBase/Hash.h"

struct PipelineKey {
	PipelineType type;

	uint64_t layout;
	uint64_t renderPass;

	ImageFormat colorFormat;
	ImageFormat depthForamt;

	std::vector<uint64_t> shadersHashes;

	RasterState raster;
	BlendState blend;

	bool operator==(const PipelineKey& rhs) const {
		return type == rhs.type &&
			layout == rhs.layout &&
			renderPass == rhs.renderPass &&
			colorFormat == rhs.colorFormat &&
			depthForamt == rhs.depthForamt &&
			shadersHashes == rhs.shadersHashes &&
			raster.depthTest == rhs.raster.depthTest &&
			raster.depthWrite == rhs.raster.depthWrite &&
			blend.enable == rhs.blend.enable;
	}
};

struct PipelineKeyHash {
	size_t operator()(const PipelineKey& key) const {
		return hashPipelineKey(key);
	}
	static size_t hashPipelineKey(const PipelineKey& key);
	static uint64_t hashShaderFile(const std::string& path);
};

