#include "PipelineHash.h"

size_t PipelineKeyHash::hashPipelineKey(const PipelineKey& key) {
	size_t hash = 0;
	hashCombine(hash, key.type);
	hashCombine(hash, key.layout);
	hashCombine(hash, key.renderPass);
	hashCombine(hash, key.colorFormat);
	hashCombine(hash, key.depthForamt);

	hashCombineRange(hash, key.shadersHashes.data(), key.shadersHashes.size());

	hashCombine(hash, key.raster);
	hashCombine(hash, key.blend);

	return hash;
}

uint64_t PipelineKeyHash::hashShaderFile(const std::string& path) {
	auto bytes = readBinaryFile(path.c_str());

	return fnv1a64(bytes.data(), bytes.size());
}