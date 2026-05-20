#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

template <typename T, typename Traits>
class static_registry {
private:
	std::vector<T> m_records;

	VkDevice r_device = VK_NULL_HANDLE;

public:
	static_registry() noexcept = default;

	static_registry(VkDevice device) noexcept
		:
		r_device(device)
	{

	}

	static_registry(const static_registry&) = delete;
	static_registry& operator=(const static_registry&) = delete;

	static_registry(static_registry&& other) noexcept
		:
		m_records(std::move(other.m_records)),
		r_device(other.r_device)
	{
		other.r_device = VK_NULL_HANDLE;
	}

	static_registry& operator=(static_registry&& other) noexcept {
		if (this == &other)
			return *this;

		reset();

		m_records = std::move(other.m_records);
		r_device = other.r_device;

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	~static_registry() noexcept {
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