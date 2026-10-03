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

	inline size_t getImageSize(const ImageInfo* const pImageInfo) noexcept {
		return (((size_t)pImageInfo->bitDepth * pImageInfo->channels * pImageInfo->width) >> 3) * pImageInfo->height;
	}

	int getBinarySize(const char* const _Path, size_t* const pSize) noexcept;
	int loadBinary(const char* const _Path, const size_t _LoadSize, void* const pDst) noexcept;

	int parseWavefront(const void* const pSrc, const size_t _Size) noexcept;

	int fetchPngInfo(const void* pBin, size_t _Size, ImageInfo* const pInfo) noexcept;

	struct ImageDecodeInfo {
		ImageInfo* imageInfo;
		const void* bin;
		size_t binSize;
		void* pDst;
	};

	int decodePng(mem_stack* pScratch, const ImageDecodeInfo* pDecodeInfo) noexcept;

}