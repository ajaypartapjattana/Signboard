#include "platform.h"

#include <stdexcept>

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProcW(hwnd, message, wParam, lParam);
	}
}

constexpr LPCWSTR className = L"RendererWindows";

void Platform::createWindowClass() {
	HINSTANCE instance = GetModuleHandleW(nullptr);

	ATOM windowClass;

	{
		WNDCLASSEXW classDesc{};
		classDesc.cbSize = sizeof(classDesc);
		classDesc.style = CS_HREDRAW | CS_VREDRAW;
		classDesc.lpfnWndProc = WindowProc;
		classDesc.cbClsExtra = 0;
		classDesc.cbWndExtra = 0;
		classDesc.hInstance = instance;
		classDesc.hIcon = nullptr;
		classDesc.hCursor = LoadCursorW(nullptr, IDC_CROSS);
		classDesc.hbrBackground = nullptr;
		classDesc.lpszMenuName = nullptr;
		classDesc.lpszClassName = className;
		classDesc.hIconSm = nullptr;

		windowClass = RegisterClassExW(&classDesc);
		if (!windowClass)
			throw std::runtime_error("FAILURE : window_class_registration!");
	}

	if (m_windowClass)
		UnregisterClassW(className, m_instance);

	m_windowClass = windowClass;

	m_instance = instance;
}

HWND Platform::createWindow(const wchar_t* title, uint32_t width, uint32_t height) noexcept {
	HWND hwnd = CreateWindowExW(0, className, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, m_instance, nullptr);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	return hwnd;
}

bool Platform::pollEvents() noexcept {
	MSG msg;

	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			return false;

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return true;
}

Platform::~Platform() noexcept {
	if (m_windowClass)
		UnregisterClassW(className, m_instance);
}
