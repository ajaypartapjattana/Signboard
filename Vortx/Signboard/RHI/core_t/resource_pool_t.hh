#pragma once

#include <vector>
#include <stdexcept>

template <typename Traits>
class resource_pool {
private:

	using _Ty = typename Traits::handle_type;
	using _parentTy = typename Traits::parent_type;
	using _resultTy = typename Traits::result_type;
	using _infoTy = typename Traits::createInfo_type;

	std::vector<_Ty> m_records;

	_parentTy r_root = Traits::null_root();

public:
	resource_pool() noexcept = default;

	resource_pool(_parentTy root) noexcept
		:
		r_root(root)
	{

	}

	resource_pool(const resource_pool&) = delete;
	resource_pool& operator=(const resource_pool&) = delete;

	resource_pool(resource_pool&& other) noexcept
		:
		m_records(std::move(other.m_records)),
		r_root(other.r_root)
	{
		other.r_root = Traits::null_root();
	}

	resource_pool& operator=(resource_pool&& other) noexcept {
		if (this == &other)
			return *this;

		reset();

		m_records = std::move(other.m_records);
		r_root = other.r_root;

		other.r_root = Traits::null_root();

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

	_resultTy create(const _infoTy* pInfo, _Ty* target) noexcept {
		_resultTy result = Traits::create(r_root, pInfo, target);
		if (result != Traits::success())
			return result;

		m_records.push_back(*target);

		return Traits::success();
	}

};