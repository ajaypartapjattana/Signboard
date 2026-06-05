#pragma once

#include <vector>
#include <memory>

template <typename Traits>
class fungible_pool {
private:

	using _Ty = typename Traits::handle_type;
	using _parentTy = typename Traits::parent_type;
	using _resultTy = typename Traits::result_type;
	using _infoTy = typename Traits::createInfo_type;

	std::vector<_Ty> m_resources;
	std::vector<uint64_t> available;
	uint32_t m_size;
	uint32_t nextFree= 0;

	std::unique_ptr<_infoTy> m_ownedInfo;
	_infoTy* pInfo = nullptr;
	
	_parentTy r_parent{};

	_infoTy* allot_basic(_infoTy* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<_infoTy>();
		return m_ownedInfo.get();
	}

public:

	fungible_pool(_parentTy parent, _infoTy* pCreateInfo = nullptr)
		:
		r_parent(parent),
		pInfo(allot_basic(pCreateInfo))
	{

	}

	~fungible_pool() noexcept {
		for (uint32_t i = 0; i < m_size; ++i) {
			Traits::destroy(r_parent, m_resources[i]);
		}
	}

	inline _Ty operator[](uint32_t index) const noexcept {
		return m_resources[index];
	}

	inline _infoTy& createInfo() noexcept {
		return *pInfo;
	}

	inline uint32_t size() const noexcept {
		return static_cast<uint32_t>(m_resources.size());
	}

	uint32_t grant() noexcept {
		const uint32_t blockCount = static_cast<uint32_t>(available.size());

		uint32_t startBlock = nextFree >> 6;

		for (uint32_t count = 0, blockIndex = startBlock; count < blockCount; ++count, blockIndex = (blockIndex + 1 == blockCount) ? 0 : blockIndex + 1) {
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

	_resultTy root(_parentTy parent) {
		std::vector<_Ty> resources(m_size);

		_resultTy result;
		for (size_t i = 0; i < m_size; ++i) {
			result = Traits::create(parent, pInfo, &resources[i]);
			if (result == Traits::success())
				continue;

			for (size_t j = 0; j < i; ++j) {
				Traits::destroy(parent, resources[j]);
			}

			return result;
		}

		for (size_t i = 0; i < m_size; ++i) {
			Traits::destroy(r_parent, m_resources[i]);
		}

		m_resources.swap(resources);
		r_parent = parent;

		return Traits::success();
	}

	_resultTy resize(uint32_t size) {
		if (size == m_size)
			return Traits::success();

		const uint32_t blockSz_64 = (size + 63u) >> 6;

		if (size < m_size) {
			for (uint32_t i = size; i < m_size; ++i) {
				Traits::destroy(r_parent, m_resources[i]);
			}

			m_resources.resize(size);
			available.resize(blockSz_64);

			if (available.empty()) {
				return Traits::success();
			}

			const uint32_t rem = size & 63u;

			if (rem) {
				available.back() &= ((1ull << rem) - 1ull);
			}

			m_size = size;

			return Traits::success();
		}

		m_resources.resize(size);

		_resultTy result;
		for (uint32_t i = m_size; i < size; ++i) {
			result = Traits::create(r_parent, pInfo, &m_resources[i]);
			if (result == Traits::success()) {
				continue;
			}

			for (uint32_t j = m_size; j < i; ++j) {
				Traits::destroy(r_parent, m_resources[j]);
			}

			m_resources.resize(m_size);

			return result;
		}

		available.resize(blockSz_64, ~0ull);

		const uint32_t startBlock = m_size >> 6;
		const uint32_t startBit = m_size & 63u;

		if (startBit) {
			available[startBlock] |= (~0ull << startBit);
		}

		const uint32_t endBit = size & 63u;

		if (endBit) {
			available.back() &= ((1ull << endBit) - 1ull);
		}

		m_size = size;

		return Traits::success();
	}

};