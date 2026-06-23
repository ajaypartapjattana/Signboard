#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <vector>

class Platform {
private:
	HINSTANCE m_instance = nullptr;
	ATOM m_windowClass = 0;

	std::vector<HWND> m_windows;

public:
	Platform() noexcept = default;

	~Platform() noexcept;

	HINSTANCE getInstance() const noexcept {
		return m_instance;
	}

	void createWindowClass();
	HWND createWindow(const wchar_t* title, uint32_t width, uint32_t height) noexcept;
	bool pollEvents() noexcept;

};