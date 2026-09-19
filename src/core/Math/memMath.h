#pragma once

#include <cstdint>

constexpr size_t alignUp(size_t _Offset, size_t _Align) noexcept {
	return (_Offset + _Align - 1) & ~(_Align - 1);
}
template <typename _Ty>
constexpr _Ty* alignUp(const void* _Addr) noexcept {
	return reinterpret_cast<_Ty*>((reinterpret_cast<uintptr_t>(_Addr) + alignof(_Ty) - 1) & ~(alignof(_Ty) - 1));
}
template <typename _Ty>
constexpr _Ty* alignUp(const void* _Addr, size_t _Align) noexcept {
	return reinterpret_cast<_Ty*>((reinterpret_cast<uintptr_t>(_Addr) + _Align - 1) & ~(_Align - 1));
}

constexpr size_t alignDown(size_t _Offset, size_t _Align) noexcept {
	return _Offset & ~(_Align - 1);
}
template <typename _Ty>
constexpr _Ty* alignDown(const void* _Addr) noexcept {
	return reinterpret_cast<_Ty*>(reinterpret_cast<uintptr_t>(_Addr) & ~(alignof(_Ty) - 1));
}
template <typename _Ty>
constexpr _Ty* alignDown(const void* _Addr, size_t _Align) noexcept {
	return reinterpret_cast<_Ty*>(reinterpret_cast<uintptr_t>(_Addr) & ~(_Align - 1));
}