#pragma once

#include <stdint.h>

#include <core/memory.h>

namespace io {

	struct ImageInfo {
		uint32_t width;
		uint32_t height;
		uint16_t channels;
		uint16_t bitDepth;
	};

	inline size_t getImageSize(const ImageInfo* pImageInfo) noexcept {
		return (((size_t)pImageInfo->bitDepth * pImageInfo->channels * pImageInfo->width) >> 3) * pImageInfo->height;
	}

	int getBinarySize(const char* path, size_t* pSize) noexcept;
	int loadBinary(const char* path, size_t loadSize, void* pDst) noexcept;

	int parseWavefront(const void* pSrc, size_t size) noexcept;

	int fetchPngInfo(const void* pBin, size_t size, ImageInfo* pInfo) noexcept;

	struct ImageDecodeInfo {
		ImageInfo* imageInfo;
		const void* bin;
		size_t binSize;
		void* pDst;
	};

	int decodePng(mem_stack* pScratch, const ImageDecodeInfo* pDecodeInfo) noexcept;

}
