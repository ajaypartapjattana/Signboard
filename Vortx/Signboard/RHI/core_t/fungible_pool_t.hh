#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

template <typename Traits>
class fungible_pool {
private:

	using _Ty = typename Traits::type;
	using _rootTy = typename Traits::root;

	std::vector<_Ty> m_resources;
	std::vector<uint64_t> available;
	uint32_t m_size;
	uint32_t nextFree= 0;

	std::unique_ptr<typename Traits::createInfo> m_ownedInfo;
	typename Traits::createInfo* pInfo = nullptr;
	
	_rootTy r_root{};

	typename Traits::createInfo* allot_basic(typename Traits::createInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<typename Traits::createInfo>();
		return m_ownedInfo.get();
	}

public:

	fungible_pool(_rootTy root, uint32_t capacity = 0, typename Traits::createInfo* pCreateInfo = nullptr) 
		:
		r_root(root),
		pInfo(allot_basic(pCreateInfo))
	{
		resize(capacity);
	}

	~fungible_pool() noexcept {
		const size_t _rSz = m_resources.size();

		for (size_t i = 0; i < _rSz; ++i) {
			Traits::destroy(r_root, m_resources[i]);
		}
	}

	inline _Ty operator[](uint32_t index) const noexcept {
		return m_resources[index];
	}

	Traits::createInfo& createInfo() noexcept {
		return *pInfo;
	}

	uint32_t size() const noexcept {
		return static_cast<uint32_t>(m_resources.size());
	}

	uint32_t grant() noexcept {
		const uint32_t blockCount = static_cast<uint32_t>(available.size());

		uint32_t startBlock = nextFree >> 6;

		for (uint32_t blockIndex = startBlock, count = 0; blockIndex < blockCount; ++count, blockIndex = (blockIndex + 1 == blockCount) ? 0 : blockIndex + 1) {
			uint64_t bits = available[blockIndex];

			if (!bits)
				continue;

#if defined(_MSC_VER)
			unsigned long bitIndex;
			_BitScanForward64(&bitIndex, bits);
			const uint32_t bit = static_cast<uint32_t>(bitIndex);
#else
			const uint32_t bit = static_cast<uint32_t>(__builtin_ctzll(bits));
#endif
			const uint32_t index = (blockIndex << 6) + bit;

			available[blockIndex] &= ~(1ull << bit);
			nextFree = index + 1;

			return index;
		}

		const uint32_t oldSize = m_size;
		resize(oldSize ? oldSize * 2 : 1);

		const uint32_t block = oldSize >> 6;
		const uint32_t bit = oldSize & 63u;

		available[block] &= ~(1ull << bit);
		nextFree = oldSize + 1;

		return oldSize;
	}

	void recycle(uint32_t index) noexcept {
		const uint32_t blockIndex = index >> 6;
		const uint32_t bit = index & 63u;

		available[blockIndex] |= (1ull << bit);

		if (index < nextFree)
			nextFree = index;
	}

	VkResult root(_rootTy root) {
		const size_t _rSz = m_resources.size();
		std::vector<_Ty> resources(_rSz);

		VkResult result;
		for (size_t i = 0; i < _rSz; ++i) {
			result = Traits::create(root, pInfo, &resources[i]);
			if (result == VK_SUCCESS)
				continue;

			for (size_t j = 0; j < i; ++j) {
				Traits::destroy(root, resources[j]);
			}

			return result;
		}

		for (size_t i = 0; i < _rSz; ++i) {
			Traits::destroy(r_root, m_resources[i]);
		}

		m_resources.swap(resources);
		r_root = root;

		return VK_SUCCESS;
	}

	VkResult resize(uint32_t size) {
		const uint32_t _cSz = static_cast<uint32_t>(m_resources.size());

		if (size == _cSz)
			return VK_SUCCESS;

		const uint32_t _blSz = (size + 63u) >> 6;

		if (size < _cSz) {
			for (uint32_t i = size; i < _cSz; ++i) {
				Traits::destroy(r_root, m_resources[i]);
			}

			m_resources.resize(size);
			available.resize(_blSz);

			if (available.empty()) {
				return VK_SUCCESS;
			}

			const uint32_t rem = size & 63u;

			if (rem) {
				available.back() &= ((1ull << rem) - 1ull);
			}

			m_size = size;

			return VK_SUCCESS;
		}

		m_resources.resize(size);

		VkResult result;
		for (uint32_t i = _cSz; i < size; ++i) {
			result = Traits::create(r_root, pInfo, &m_resources[i]);
			if (result == VK_SUCCESS) {
				continue;
			}

			for (uint32_t j = _cSz; j < i; ++j) {
				Traits::destroy(r_root, m_resources[j]);
			}

			m_resources.resize(_cSz);

			return result;
		}

		available.resize(_blSz, ~0ull);

		const uint32_t startBlock = _cSz >> 6;
		const uint32_t startBit = _cSz & 63u;

		if (startBit) {
			available[startBlock] |= (~0ull << startBit);
		}

		const uint32_t endBit = size & 63u;

		if (endBit) {
			available.back() &= ((1ull << endBit) - 1ull);
		}

		m_size = size;

		return VK_SUCCESS;
	}

};