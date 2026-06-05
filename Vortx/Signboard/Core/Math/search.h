#pragma once

#include <unordered_set>

namespace std {

	struct DefaultEq {
		template <typename T>
		bool operator()(const T& a, const T& b) const noexcept {
			return a == b;
		}
	};

	template <typename T, typename _Teq = DefaultEq>
	bool findEarly_basic(const T* pList, size_t listSize, const T* pSearch, size_t searchSize, size_t* index) noexcept {
		_Teq eq{};
		
		for (size_t i = 0; i < listSize; ++i)
			for (size_t j = 0; j < searchSize; ++j)
				if (eq(pList[i], pSearch[j])) {
					if(index)
						*index = j;
					return true;
				}

		return false;
	}

	template <typename _Key, typename _Hasher, typename _Teq>
	const _Key* findEarly_Hashed(const _Key* pList, size_t listSize, const _Key* pSearch, size_t searchSize) noexcept {
		std::unordered_set<_Key, _Hasher, _Teq> searchSet;

		for (size_t j = 0; j < searchSize; ++j) {
			searchSet.insert(pSearch[j]);
		}

		for (size_t i = 0; i < listSize; ++i) {
			if (searchSet.find(pList[i]) != searchSet.end()) {
				return &pList[i];
			}
		}

		return nullptr;
	}

}

