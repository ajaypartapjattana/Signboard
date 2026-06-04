#pragma once

#include <vector>
#include <stdexcept>

template <typename Traits>
class contextual_pool {
private:

	using _Ty = typename Traits::handle_type;
	using _parentTy = typename Traits::parent_type;
	using _resultTy = typename Traits::result_type;
	using _infoTy = typename Traits::createInfo_type;

	std::vector<_Ty> m_records;

	_parentTy r_root = Traits::null_root();

public:
	contextual_pool() noexcept = default;
	contextual_pool(_parentTy root) noexcept
		:
		r_root(root)
	{

	}
	contextual_pool(const contextual_pool&) = delete;
	contextual_pool(contextual_pool&& other) noexcept
		:
		m_records(std::move(other.m_records)),
		r_root(other.r_root)
	{
		other.r_root = Traits::null_root();
	}

	contextual_pool& operator=(const contextual_pool&) = delete;
	contextual_pool& operator=(contextual_pool&& other) noexcept {
		if (this == &other)
			return *this;

		reset();

		m_records = std::move(other.m_records);
		r_root = other.r_root;

		other.r_root = Traits::null_root();

		return *this;
	}

	~contextual_pool() noexcept {
		reset();
	}

	void reset() noexcept {
		const size_t size = m_records.size();

		for (size_t i = 0; i < size; ++i) {
			_Ty record = m_records[i];

			if (record == Traits::null())
				continue;

			Traits::destroy(r_root, record);
		}

		m_records.clear();
	}

	_Ty operator[](size_t index) const noexcept {
		return m_records[index];
	}

	void root(_parentTy root) noexcept {
		reset();
		r_root = root;
	}

	_resultTy create(const _infoTy* pInfo, size_t* index) noexcept {
		_Ty target = Traits::null();
		_resultTy result = Traits::create(r_root, pInfo, &target);
		if (result != Traits::success())
			return result;

		size_t pushIndex = m_records.size();

		m_records.push_back(target);
		*index = pushIndex;

		return Traits::success();
	}

	_resultTy replace(const _infoTy* pInfo, size_t index) noexcept {
		_Ty target = Traits::null();
		_resultTy result = Traits::create(r_root, pInfo, &target);
		if (result != Traits::success())
			return result;

		Traits::destroy(r_root, m_records[index]);
		m_records[index] = target;

		return Traits::success();
	}

};