#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>

template <typename T, typename Meta>
struct resource_record {
	T handle = VK_NULL_HANDLE;
	
	Meta meta{};
};

template <typename T, typename Meta, typename Traits>
class contextual_pool {
private:
	std::vector<resource_record<T, Meta>> m_records;
	std::unordered_map<T, size_t> mapping;

	VkDevice r_device = VK_NULL_HANDLE;

public:
	contextual_pool() noexcept = default;

	contextual_pool(VkDevice device) noexcept
		:
		r_device(device)
	{

	}

	contextual_pool(const contextual_pool&) = delete;
	contextual_pool& operator=(const contextual_pool&) = delete;

	contextual_pool(contextual_pool&& other) noexcept
		:
		m_records(std::move(other.m_records)),
		r_device(other.r_device)
	{
		other.r_device = VK_NULL_HANDLE;
	}

	contextual_pool& operator=(contextual_pool&& other) noexcept {
		if (this == &other)
			return *this;

		clear();

		m_records = std::move(other.m_records);
		r_device = other.r_device;

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	~contextual_pool() noexcept {
		clear();
	}

	void clear() noexcept {
		const size_t _rSz = m_records.size();
		
		for (size_t i = 0; i < _rSz; ++i) {
			Traits::destroy(r_device, m_records[i].handle);
		}

		m_records.clear()
	}

	using resource_id = uint32_t;

	T create(const Meta& meta, VkResult* pResult = nullptr) {
		resource_record<T, Meta> rec{};
		rec.meta = meta;

		VkResult result;
		result = Traits::create(r_device, meta, &rec.handle);

		if (pResult)
			*pResult = result;

		if (result != VK_SUCCESS)
			return VK_NULL_HANDLE;

		m_records.push_back(rec);

		return rec.handle;
	}

	const Meta& meta(T handle) const noexcept {
		return m_records[mapping.find(handle)->second].meta;
	}
	

};