#pragma once

#include <vector>
#include <stdexcept>

template <typename Traits>
class resource_pool {
private:

	using _Ty = typename Traits::handle_type;
	using _rootTy = typename Traits::root_type;
	using _resultTy = typename Traits::result_type;
	using _infoTy = typename Traits::createInfo_type;

	std::vector<_Ty> m_records;

	_rootTy r_root = VK_NULL_HANDLE;

public:
	resource_pool() noexcept = default;

	resource_pool(_rootTy root) noexcept
		:
		r_root(root)
	{

	}

	resource_pool(const resource_pool&) = delete;
	resource_pool& operator=(const resource_pool&) = delete;

	resource_pool(resource_pool&& other) noexcept
		:
		m_records(std::move(other.m_records)),
		r_root(other.r_device)
	{
		other.r_root = VK_NULL_HANDLE;
	}

	resource_pool& operator=(resource_pool&& other) noexcept {
		if (this == &other)
			return *this;

		reset();

		m_records = std::move(other.m_records);
		r_root = other.r_root;

		other.r_root = VK_NULL_HANDLE;

		return *this;
	}

	~resource_pool() noexcept {
		reset();
	}

	void reset() noexcept {
		const size_t size = m_records.size();

		for (size_t i = 0; i < size; ++i) {
			Traits::destroy(r_root, m_records[i]);
		}

		m_records.clear();
	}

	_resultTy create(Traits::createInfo* pInfo, _Ty& target) {
		_resultTy result = Traits::create(r_root, pInfo, &target);
		if (result != _resultTy{})
			return result;

		m_records.push_back(target);

		return _resultTy{};
	}

};