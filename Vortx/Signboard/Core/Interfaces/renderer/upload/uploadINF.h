#pragma once

#include <cstdint>
#include <vector>

struct UploadSpan {
	const void* data = nullptr;
	size_t size = 0;
	size_t alignment = 1;

	UploadSpan() = default;

	template <typename T>
	UploadSpan(const std::vector<T>& v)
		:
		data(v.empty() ? nullptr : v.data()),
		size(v.size() * sizeof(T)),
		alignment(alignof(T))
	{

	}

	template <typename T>
	UploadSpan(const T* ptr, size_t count)
		:
		data(ptr),
		size(count * sizeof(T)),
		alignment(alignof(T))
	{

	}

	template <typename T>
	UploadSpan(const T& obj)
		:
		data(&obj),
		size(sizeof(T)),
		alignment(alignof(T))
	{

	}

	bool valid() const noexcept {
		return data != nullptr && size > 0;
	}
};

struct UploadTarget {
	uint32_t buffer;
	uint64_t offset;
};

struct UploadOp {
	UploadSpan src;
	UploadTarget dst;
};

using UploadBatch = std::vector<UploadOp>;