#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

template <typename T, typename Traits>
class fungible_pool {
private:
	std::vector<T> m_resources;
	uint32_t m_size;
	uint32_t next = 0;

	std::unique_ptr<typename Traits::createInfo> m_ownedInfo;
	typename Traits::createInfo* pInfo = nullptr;
	
	VkDevice r_device = VK_NULL_HANDLE;

	typename Traits::createInfo* allot_basic(typename Traits::createInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<typename Traits::createInfo>();
		return m_ownedInfo.get();
	}

public:

	fungible_pool(VkDevice device, uint32_t capacity = 0, typename Traits::createInfo* pCreateInfo = nullptr) 
		:
		r_device(device),
		pInfo(allot_basic(pCreateInfo))
	{
		resize(capacity);
	}

	~fungible_pool() noexcept {
		const size_t _rSz = m_resources.size();

		for (size_t i = 0; i < _rSz; ++i) {
			Traits::destroy(r_device, m_resources[i]);
		}
	}

	uint32_t size() const noexcept {
		return static_cast<uint32_t>(m_resources.size());
	}

	T grant() const noexcept {
		if (next >= m_size)
			resize(m_size ? m_size * 2 : 1);

		return m_resources[next++];
	}

	void recycle() noexcept {
		next = 0;
	}

	VkResult root(VkDevice device) {
		const size_t _rSz = m_resources.size();
		std::vector<T> resources(_rSz);

		VkResult result;
		for (size_t i = 0; i < _rSz; ++i) {
			result = Traits::create(r_device, pInfo, &m_resources[i]);
			if (result == VK_SUCCESS)
				continue;

			for (size_t j = 0; j < i; ++j) {
				Traits::destroy(r_device, m_resources[j]);
			}

			return result;
		}

		for (size_t i = 0; i < _rSz; ++i) {
			Traits::destroy(r_device, m_resources[i]);
		}

		m_resources.swap(resources);
		r_device = device;

		return VK_SUCCESS;
	}

	VkResult resize(uint32_t size) {
		const uint32_t _rSz = static_cast<uint32_t>(m_resources.size());

		if (size == _rSz)
			return VK_SUCCESS;

		if (size < _rSz) {
			for (uint32_t i = size; i < _rSz; ++i) {
				Traits::destroy(r_device, m_resources[i]);
			}

			m_resources.resize(size);

			return VK_SUCCESS;
		}

		m_resources.resize(size);

		VkResult result;
		for (uint32_t i = _rSz; i < size; ++i) {
			result = Traits::create(r_device, pInfo, &m_resources[i]);
			if (result == VK_SUCCESS) {
				continue;
			}

			for (uint32_t j = _rSz; j < i; ++j) {
				Traits::destroy(r_device, m_resources[j]);
			}

			m_resources.resize(_rSz);

			return result;
		}

		m_size = size;

		return VK_SUCCESS;
	}

};