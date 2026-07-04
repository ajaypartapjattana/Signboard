#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <cstring>
#include <cstdint>
#include <utility>

constexpr static size_t alignUp(size_t _offset, size_t _alignment) noexcept {
	return (_offset + _alignment - 1) & ~(_alignment - 1);
}

class MemoryBlock {
private:
	uint8_t* pBegin = nullptr;
	uint8_t* pCommit = nullptr;
	uint8_t* pReserve = nullptr;

	struct SystemMemoryInfo {
		size_t pageSize;
		size_t reservationGranularity;
	};

	inline static const SystemMemoryInfo memInfo = [] {
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		return SystemMemoryInfo{ 
			(size_t)info.dwPageSize, 
			(size_t)info.dwAllocationGranularity 
		};
	}();

public:
	MemoryBlock() noexcept = default;
	MemoryBlock(size_t _Reserve, size_t _Size) {
		reserve(_Reserve);
		commit(_Size);
	}
	MemoryBlock(const MemoryBlock& _Other) noexcept = delete;
	MemoryBlock(MemoryBlock&& _Other) noexcept
		:
		pBegin(std::exchange(_Other.pBegin, nullptr)),
		pCommit(std::exchange(_Other.pCommit, nullptr)),
		pReserve(std::exchange(_Other.pReserve, nullptr))
	{

	}

	MemoryBlock& operator=(const MemoryBlock& _Other) noexcept = delete;
	MemoryBlock& operator=(MemoryBlock&& _Other) noexcept {
		if (this == &_Other)
			return *this;

		if (pBegin)
			VirtualFree(pBegin, 0, MEM_RELEASE);

		pBegin = std::exchange(_Other.pBegin, nullptr);
		pCommit = std::exchange(_Other.pCommit, nullptr);
		pReserve = std::exchange(_Other.pReserve, nullptr);

		return *this;
	}

	~MemoryBlock() noexcept {
		if (pBegin)
			VirtualFree(pBegin, 0, MEM_RELEASE);
	}

	uint8_t* data() noexcept {
		return pBegin;
	}

	const uint8_t* data() const noexcept {
		return pBegin;
	}

	size_t size() const noexcept {
		return static_cast<size_t>(pCommit - pBegin);
	}

	void reserve(size_t _Capacity) {
		if (pBegin)
			VirtualFree(pBegin, 0, MEM_RELEASE);

		_Capacity = alignUp(_Capacity, memInfo.reservationGranularity);

		void* pAlloc = VirtualAlloc(nullptr, _Capacity, MEM_RESERVE, PAGE_READWRITE);

		if (!pAlloc)
			throw std::bad_alloc();

		pBegin = static_cast<uint8_t*>(pAlloc);
		pCommit = static_cast<uint8_t*>(pAlloc);
		pReserve = pBegin + _Capacity;
	}

	void commit(size_t _Size) {
		_Size = alignUp(_Size, memInfo.pageSize);

		if (pCommit + _Size > pReserve)
			throw std::bad_alloc();

		uint8_t* _memory = static_cast<uint8_t*>(VirtualAlloc(pCommit, _Size, MEM_COMMIT, PAGE_READWRITE));

		if (!_memory)
			throw std::bad_alloc();

		pCommit += _Size;
	}

	void clone(const MemoryBlock& _Other) {
		reserve(_Other.pReserve - _Other.pBegin);

		const size_t cloneSize = _Other.pCommit - _Other.pBegin;
		commit(cloneSize);

		std::memcpy(pBegin, _Other.pBegin, cloneSize);
	}

};

class LinearAllocator {
private:
	MemoryBlock* pMemory = nullptr;
	uint8_t* pCurrent = nullptr;

public:
	LinearAllocator() noexcept = default;
	LinearAllocator(MemoryBlock* pMemoryBlock) noexcept
		:
		pMemory(pMemoryBlock),
		pCurrent(pMemoryBlock->data())
	{

	}

	void allocate(size_t _Size, size_t _Align) {

	}

};