#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

template <typename T, typename Traits>
class resource_pool {
private:
	std::vector<T> m_records;

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

		reset();

		m_records = std::move(other.m_records);
		r_device = other.r_device;

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	~resource_pool() noexcept {
		reset();
	}

	void reset() noexcept {
		const size_t _rSz = m_records.size();

		for (size_t i = 0; i < _rSz; ++i) {
			Traits::destroy(r_device, m_records[i]);
		}

		m_records.clear();
	}

	T create(Traits::createInfo* pCreateInfo) {
		T handle;
		VkResult result = Traits::create(r_device, pCreateInfo, &handle);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: vulkan_type_create!");

		m_records.push_back(handle);

		return handle;
	}

};