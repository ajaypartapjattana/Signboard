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
class resource_pool {
private:
	std::vector<resource_record<T, Meta>> m_records;
	std::unordered_map<T, size_t> mapping;

	VkDevice r_device = VK_NULL_HANDLE;

public:
	resource_pool() noexcept = default;

	resource_pool(VkDevice device) noexcept
		:
		r_device(device)
	{

	}

	resource_pool(const resource_pool&) = delete;
	resource_pool& operator=(const resource_pool&) = delete;

	resource_pool(resource_pool&& other) noexcept
		:
		m_records(std::move(other.m_records)),
		r_device(other.r_device)
	{
		other.r_device = VK_NULL_HANDLE;
	}

	resource_pool& operator=(resource_pool&& other) noexcept {
		if (this == &other)
			return *this;

		clear();

		m_records = std::move(other.m_records);
		r_device = other.r_device;

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	~resource_pool() noexcept {
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