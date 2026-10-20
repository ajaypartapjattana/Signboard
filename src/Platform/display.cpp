#include <stdlib.h>
#include <new>

#include <vulkan/vulkan.h>

#include <core/memory.h>

#include "display.h"

#if defined(PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <windowsx.h>

constexpr Key normalKeys[0x59] = {
	KEY_IMP,				// 0x00
	KEY_ESCAPE,             // 0x01
	KEY_1,                  // 0x02
	KEY_2,                  // 0x03
	KEY_3,                  // 0x04
	KEY_4,                  // 0x05
	KEY_5,                  // 0x06
	KEY_6,                  // 0x07
	KEY_7,                  // 0x08
	KEY_8,                  // 0x09
	KEY_9,                  // 0x0A
	KEY_0,                  // 0x0B
	KEY_MINUS,              // 0x0C
	KEY_EQUALS,             // 0x0D
	KEY_BACKSPACE,          // 0x0E
	KEY_TAB,                // 0x0F
	KEY_Q,                  // 0x10
	KEY_W,                  // 0x11
	KEY_E,                  // 0x12
	KEY_R,                  // 0x13
	KEY_T,                  // 0x14
	KEY_Y,                  // 0x15
	KEY_U,                  // 0x16
	KEY_I,                  // 0x17
	KEY_O,                  // 0x18
	KEY_P,                  // 0x19
	KEY_LEFT_BRACKET,       // 0x1A
	KEY_RIGHT_BRACKET,      // 0x1B
	KEY_ENTER,              // 0x1C
	KEY_CTRL_LEFT,          // 0x1D
	KEY_A,                  // 0x1E
	KEY_S,                  // 0x1F
	KEY_D,                  // 0x20
	KEY_F,                  // 0x21
	KEY_G,                  // 0x22
	KEY_H,                  // 0x23
	KEY_J,                  // 0x24
	KEY_K,                  // 0x25
	KEY_L,                  // 0x26
	KEY_SEMICOLON,          // 0x27
	KEY_APOSTROPHE,         // 0x28
	KEY_GRAVE,              // 0x29
	KEY_SHIFT_LEFT,         // 0x2A
	KEY_BACKSLASH,          // 0x2B
	KEY_Z,                  // 0x2C
	KEY_X,                  // 0x2D
	KEY_C,                  // 0x2E
	KEY_V,                  // 0x2F
	KEY_B,                  // 0x30
	KEY_N,                  // 0x31
	KEY_M,                  // 0x32
	KEY_COMMA,              // 0x33
	KEY_PERIOD,             // 0x34
	KEY_SLASH,              // 0x35
	KEY_SHIFT_RIGHT,        // 0x36
	KEY_NUMPAD_MULTIPLY,    // 0x37
	KEY_ALT_LEFT,           // 0x38
	KEY_SPACE,              // 0x39
	KEY_CAPSLOCK,			// 0x3A
	KEY_F1,                 // 0x3B
	KEY_F2,                 // 0x3C
	KEY_F3,                 // 0x3D
	KEY_F4,                 // 0x3E
	KEY_F5,                 // 0x3F
	KEY_F6,                 // 0x40
	KEY_F7,                 // 0x41
	KEY_F8,                 // 0x42
	KEY_F9,                 // 0x43
	KEY_F10,                // 0x44
	KEY_NUMLOCK,			// 0x45
	KEY_SCROLL_LOCK,        // 0x46
	KEY_NUMPAD_7,           // 0x47
	KEY_NUMPAD_8,           // 0x48
	KEY_NUMPAD_9,           // 0x49
	KEY_NUMPAD_SUBTRACT,    // 0x4A
	KEY_NUMPAD_4,           // 0x4B
	KEY_NUMPAD_5,           // 0x4C
	KEY_NUMPAD_6,           // 0x4D
	KEY_NUMPAD_ADD,         // 0x4E
	KEY_NUMPAD_1,           // 0x4F
	KEY_NUMPAD_2,           // 0x50
	KEY_NUMPAD_3,           // 0x51
	KEY_NUMPAD_0,           // 0x52
	KEY_NUMPAD_DECIMAL,     // 0x53
	KEY_IMP,				// 0x54
	KEY_IMP,				// 0x55
	KEY_NON_US_BACKSLASH,   // 0x56
	KEY_F11,                // 0x57
	KEY_F12                 // 0x58
};

constexpr Key extendedKeys[0x54] = {
	KEY_IMP,                // 0x00
	KEY_IMP,                // 0x01
	KEY_IMP,                // 0x02
	KEY_IMP,                // 0x03
	KEY_IMP,                // 0x04
	KEY_IMP,                // 0x05
	KEY_IMP,                // 0x06
	KEY_IMP,                // 0x07
	KEY_IMP,                // 0x08
	KEY_IMP,                // 0x09
	KEY_IMP,                // 0x0A
	KEY_IMP,                // 0x0B
	KEY_IMP,                // 0x0C
	KEY_IMP,                // 0x0D
	KEY_IMP,                // 0x0E
	KEY_IMP,                // 0x0F
	KEY_IMP,                // 0x10
	KEY_IMP,                // 0x11
	KEY_IMP,                // 0x12
	KEY_IMP,                // 0x13
	KEY_IMP,                // 0x14
	KEY_IMP,                // 0x15
	KEY_IMP,                // 0x16
	KEY_IMP,                // 0x17
	KEY_IMP,                // 0x18
	KEY_IMP,                // 0x19
	KEY_IMP,                // 0x1A
	KEY_IMP,                // 0x1B
	KEY_NUMPAD_ENTER,       // 0x1C
	KEY_CTRL_RIGHT,         // 0x1D
	KEY_IMP,                // 0x1E
	KEY_IMP,                // 0x1F
	KEY_IMP,                // 0x20
	KEY_IMP,                // 0x21
	KEY_IMP,                // 0x22
	KEY_IMP,                // 0x23
	KEY_IMP,                // 0x24
	KEY_IMP,                // 0x25
	KEY_IMP,                // 0x26
	KEY_IMP,                // 0x27
	KEY_IMP,                // 0x28
	KEY_IMP,                // 0x29
	KEY_IMP,                // 0x2A
	KEY_IMP,                // 0x2B
	KEY_IMP,                // 0x2C
	KEY_IMP,                // 0x2D
	KEY_IMP,                // 0x2E
	KEY_IMP,                // 0x2F
	KEY_IMP,                // 0x30
	KEY_IMP,                // 0x31
	KEY_IMP,                // 0x32
	KEY_IMP,                // 0x33
	KEY_IMP,                // 0x34
	KEY_NUMPAD_DIVIDE,      // 0x35
	KEY_IMP,                // 0x36
	KEY_PRINT_SCREEN,       // 0x37
	KEY_ALT_RIGHT,          // 0x38
	KEY_IMP,                // 0x39
	KEY_IMP,                // 0x3A
	KEY_IMP,                // 0x3B
	KEY_IMP,                // 0x3C
	KEY_IMP,                // 0x3D
	KEY_IMP,                // 0x3E
	KEY_IMP,                // 0x3F
	KEY_IMP,                // 0x40
	KEY_IMP,                // 0x41
	KEY_IMP,                // 0x42
	KEY_IMP,                // 0x43
	KEY_IMP,                // 0x44
	KEY_IMP,                // 0x45
	KEY_IMP,                // 0x46
	KEY_HOME,               // 0x47
	KEY_ARROW_UP,           // 0x48
	KEY_PAGE_UP,            // 0x49
	KEY_IMP,                // 0x4A
	KEY_ARROW_LEFT,         // 0x4B
	KEY_IMP,                // 0x4C
	KEY_ARROW_RIGHT,        // 0x4D
	KEY_IMP,                // 0x4E
	KEY_END,                // 0x4F
	KEY_ARROW_DOWN,         // 0x50
	KEY_PAGE_DOWN,          // 0x51
	KEY_INSERT,             // 0x52
	KEY_DELETE              // 0x53
};

static void inputProc(WindowInput* event, const RAWINPUT* const input) noexcept {
	switch (input->header.dwType) {
	case RIM_TYPEMOUSE: {
		const RAWMOUSE* const mouse = &input->data.mouse;

		if (mouse->lLastX || mouse->lLastY) {
			if (mouse->usFlags & MOUSE_MOVE_ABSOLUTE)
				event->point(static_cast<int32_t>(mouse->lLastX), static_cast<int32_t>(mouse->lLastY));
			else
				event->drag(static_cast<int32_t>(mouse->lLastX), static_cast<int32_t>(mouse->lLastY));
		}

		if (mouse->usFlags & RI_MOUSE_WHEEL)
			event->scroll(static_cast<int32_t>(mouse->usButtonData));
		
		if (!mouse->usButtonFlags)
			return;

		const USHORT flags = mouse->usButtonFlags;

		if (flags & RI_MOUSE_LEFT_BUTTON_DOWN)
			event->press(BUTTON_LEFT);
		if (flags & RI_MOUSE_LEFT_BUTTON_UP)
			event->release(BUTTON_LEFT);

		if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
			event->press(BUTTON_RIGHT);
		if (flags & RI_MOUSE_RIGHT_BUTTON_UP)
			event->release(BUTTON_RIGHT);

		if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
			event->press(BUTTON_MIDDLE);
		if (flags & RI_MOUSE_MIDDLE_BUTTON_UP)
			event->release(BUTTON_MIDDLE);

		if (flags & RI_MOUSE_BUTTON_4_DOWN)
			event->press(BUTTON_4);
		if (flags & RI_MOUSE_BUTTON_4_UP)
			event->release(BUTTON_4);

		if (flags & RI_MOUSE_BUTTON_5_DOWN)
			event->press(BUTTON_5);
		if (flags & RI_MOUSE_BUTTON_5_UP)
			event->release(BUTTON_5);
	}

		return;

	case RIM_TYPEKEYBOARD: {
		const uint16_t scanCode = input->data.keyboard.MakeCode;
		const uint16_t flags = input->data.keyboard.Flags;

		const Action action = flags & RI_KEY_BREAK ? ACTION_RELEASE : ACTION_PRESS;

		if (scanCode >= 0x80)
			return;

		Key key = KEY_IMP;

		if (flags & RI_KEY_E1) {
			if (scanCode == 0x45)
				key = KEY_PAUSE;
		}
		else if (flags & RI_KEY_E0) {
			key = extendedKeys[scanCode];
		}
		else
			key = normalKeys[scanCode];

		if (key == KEY_IMP)
			return;

		switch (action) {
		case ACTION_NONE:
			return;
		case ACTION_PRESS:
			event->press(key);
			return;
		case ACTION_REPEAT:
			event->repeat(key);
			return;
		case ACTION_RELEASE:
			event->release(key);
		}
	}

		return;

	case RIM_TYPEHID:
		return;
	}
}

struct DisplayWindow_T {
	HWND window;
	void* user;
	WindowState state;
	WindowNotification notifs;
	WindowGeomentry geometry;
	WindowInput* event;
};

static LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept {
	DisplayWindow window = reinterpret_cast<DisplayWindow>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

	if (message == WM_NCCREATE) {
		const CREATESTRUCTA* create = reinterpret_cast<const CREATESTRUCTA*>(lParam);

		window = static_cast<DisplayWindow>(create->lpCreateParams);

		SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
	}

	if (!window)
		return DefWindowProcA(hwnd, message, wParam, lParam);

	switch (message) {
	case WM_INPUT: {
		UINT size = 0;
		
		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == static_cast<UINT>(-1))
			return 0;

		alignas(RAWINPUT) uint8_t buffer[sizeof(RAWINPUT) + 64u];

		if (size > sizeof(buffer))
			return 0;

		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER)) == static_cast<UINT>(-1))
			return 0;

		window->notifs |= WINDOW_NOTIFY_INPUT_BIT;

		const RAWINPUT* input = reinterpret_cast<const RAWINPUT*>(buffer);

		inputProc(window->event, input);
	}

		return 0;

	case WM_SIZE:
		window->notifs |= WINDOW_NOTIFY_STATE_BIT;

		window->geometry.width = LOWORD(lParam);
		window->geometry.height = HIWORD(lParam);

		switch (wParam) {
		case SIZE_RESTORED:
			window->state = WINDOW_STATE_WINDOWED;
			return 0;
		case SIZE_MINIMIZED:
			window->state = WINDOW_STATE_MINIMIZED;
			return 0;
		case SIZE_MAXIMIZED:
			window->state = WINDOW_STATE_MAXIMIZED;
			return 0;
		default:
			return 0;
		}
	case WM_MOVE:
		window->notifs |= WINDOW_NOTIFY_MOVE_BIT;

		window->geometry.x = GET_X_LPARAM(lParam);
		window->geometry.y = GET_Y_LPARAM(lParam);
		return 0;
	case WM_SETFOCUS:
		window->notifs |= WINDOW_NOTIFY_FOCUS_GAIN_BIT;
		return 0;
	case WM_KILLFOCUS:
		window->notifs |= WINDOW_NOTIFY_FOCUS_LOST_BIT;
		return 0;
	case WM_CLOSE:
		window->notifs |= WINDOW_NOTIFY_TERMINATION_BIT;
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcA(hwnd, message, wParam, lParam);
}

constexpr LPCSTR className = "VortxWindows";

struct DisplayContext_T {
	HINSTANCE instance;
	LPCSTR className;
};

int requestDisplayContext(DisplayContext* pContext) noexcept {
	const DisplayContext context = new(std::nothrow) DisplayContext_T;

	if (!context)
		return -1;

	context->instance = GetModuleHandleA(nullptr);

	{
		WNDCLASSEXA classDesc{};
		classDesc.cbSize = sizeof(classDesc);
		classDesc.style = CS_HREDRAW | CS_VREDRAW;
		classDesc.lpfnWndProc = windowProc;
		classDesc.cbClsExtra = 0;
		classDesc.cbWndExtra = 0;
		classDesc.hInstance = context->instance;
		classDesc.hIcon = nullptr;
		classDesc.hCursor = LoadCursorA(nullptr, IDC_CROSS);
		classDesc.hbrBackground = nullptr;
		classDesc.lpszMenuName = nullptr;
		classDesc.lpszClassName = className;
		classDesc.hIconSm = nullptr;

		if (!RegisterClassExA(&classDesc))
			goto failure_0;
	}

	context->className = className;

	*pContext = context;

	return 0;

failure_0:
	delete context;

	return -1;
}

void destroyDisplayContext(DisplayContext context) noexcept {
	UnregisterClassA(context->className, context->instance);
}

int createDisplayWindow(DisplayContext context, const WindowCreateInfo* pCreateInfo, DisplayWindow* pWindow) noexcept {
	const DisplayWindow window = new(std::nothrow) DisplayWindow_T{};

	if (!window)
		return -1;
	
	DWORD style;

	if (pCreateInfo->flags & WINDOW_CREATE_FULLSCREEN_BIT) {
		style = WS_POPUP;

		HMONITOR monitor = MonitorFromPoint({ pCreateInfo->geometry.x, pCreateInfo->geometry.y }, MONITOR_DEFAULTTOPRIMARY);

		MONITORINFO info{};
		info.cbSize = sizeof(MONITORINFO);

		if (!GetMonitorInfoA(monitor, &info))
			goto failure_0;

		window->geometry.x = static_cast<uint16_t>(info.rcMonitor.left);
		window->geometry.y = static_cast<uint16_t>(info.rcMonitor.top);
		window->geometry.width = static_cast<uint16_t>(info.rcMonitor.right - info.rcMonitor.left);
		window->geometry.height = static_cast<uint16_t>(info.rcMonitor.bottom - info.rcMonitor.top);
	}
	else {
		style = WS_OVERLAPPEDWINDOW;
		
		if (pCreateInfo->flags & WINDOW_CREATE_STATIC_GEOMETRY_BIT)
			style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

		window->geometry = pCreateInfo->geometry;
	}

	window->window = CreateWindowExA(0, context->className, pCreateInfo->title, style, window->geometry.x, window->geometry.y, window->geometry.width, window->geometry.height, nullptr, nullptr, context->instance, window);

	if(!window->window)
		goto failure_0;

	{
		RAWINPUTDEVICE inputDevice[2]{};
		inputDevice[0].usUsagePage = 0x01;
		inputDevice[0].usUsage = 0x06;
		inputDevice[0].dwFlags = 0;
		inputDevice[0].hwndTarget = window->window;

		inputDevice[1].usUsagePage = 0x01;
		inputDevice[1].usUsage = 0x02;
		inputDevice[1].dwFlags = 0;
		inputDevice[1].hwndTarget = window->window;

		if (!RegisterRawInputDevices(inputDevice, 2u, sizeof(RAWINPUTDEVICE)))
			goto failure_0;
	}
	
	SetWindowLongPtrA(window->window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
	
	window->event = pCreateInfo->pInput;

	*pWindow = window;

	return 0;

failure_0:
	delete window;

	return -1;
}

void destroyDisplayWindow(DisplayWindow window) noexcept {
	DestroyWindow(window->window);
}

void raiseDisplayWindow(DisplayWindow window) noexcept {
	ShowWindow(window->window, SW_SHOWNORMAL);
	SetForegroundWindow(window->window);
	UpdateWindow(window->window);
}

void queryWindowGeometry(DisplayWindow window, WindowGeomentry* pGeometry) noexcept {
	*pGeometry = window->geometry;
}

int setWindowGeometry(DisplayWindow window, const WindowGeomentry* pGeometry) noexcept {
	if (!SetWindowPos(window->window, nullptr, pGeometry->x, pGeometry->y, pGeometry->width, pGeometry->height, SWP_NOZORDER | SWP_NOACTIVATE))
		return -1;

	return 0;
}

void setWindowTitle(DisplayWindow window, const char* title) noexcept {
	SetWindowTextA(window->window, title);
}

float getWindowAspect(DisplayWindow window) noexcept {
	return window->geometry.width / (float)window->geometry.height;
}

void pollWindowEvents() noexcept {
	MSG message;

	while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}
}

WindowNotification queryWindowNotification(DisplayWindow window) noexcept {
	return window->notifs;
}

void clearWindowNotification(DisplayWindow window) noexcept {
	window->notifs = 0;
}

WindowState getWindowState(DisplayWindow window) noexcept {
	return window->state;
}

void waitWindowEvents() noexcept {
	MSG message;

	while (GetMessageA(&message, nullptr, 0, 0) > 0) {
		TranslateMessage(&message);
		DispatchMessageA(&message);

		break;
	}
}

void getVulkanSurfaceDependencyInfo(DisplayContext context, DisplayWindow window, VulkanSurfaceDependencyInfo* pDependencyInfo) noexcept {
	pDependencyInfo->context = reinterpret_cast<void*>(context->instance);
	pDependencyInfo->window = reinterpret_cast<uintptr_t>(window->window);
}

#elif defined(PLATFORM_LINUX)

#if defined(WINDOW_XCB)

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <vulkan/vulkan_xcb.h>

static xcb_atom_t internAtom(xcb_connection_t* const pConnection, const char* _Name) {
	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(pConnection, 0, strlen(_Name), _Name);

	xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(pConnection, cookie, nullptr);

	if (!reply)
		return XCB_ATOM_NONE;

	const xcb_atom_t atom = reply->atom;
	std::free(reply);
	
	return atom;
}

struct DisplayContext_T {
	xcb_connection_t* connection;

	xcb_atom_t wmProtocols;
	xcb_atom_t wmDeleteWindow;
	xcb_atom_t wmState;
	xcb_atom_t netActiveWindow;
};

int requestDisplayContext(DisplayContext* const pContext) noexcept {
	xcb_connection_t* _connection = nullptr;

	do {
		_connection = xcb_connect(nullptr, nullptr);

		if (xcb_connection_has_error(_connection))
			break;

		const xcb_atom_t wmProtocols = internAtom(_connection, "WM_PROTOCOLS");

		if (wmProtocols == XCB_ATOM_NONE)
			break;

		const xcb_atom_t wmDeleteWindow = internAtom(_connection, "WM_DELETE_WINDOW");

		if (wmDeleteWindow == XCB_ATOM_NONE)
			break;

		const xcb_atom_t wmState = internAtom(_connection, "WM_STATE");

		if (wmState == XCB_ATOM_NONE)
			break;

		const xcb_atom_t netActiveWindow = internAtom(_connection, "_NET_ACTIVE_WINDOW");

		if (netActiveWindow == XCB_ATOM_NONE)
			break;

		DisplayContext const context = new(std::nothrow) DisplayContext_T;
		
		if (!pContext)
			break;

		context->netActiveWindow = netActiveWindow;
		context->wmState = wmState;
		context->wmProtocols = wmProtocols;
		context->wmDeleteWindow = wmDeleteWindow;
			
		context->connection = _connection;

		*pContext = context;	

		return 0;

	} while (false);

	if (_connection)
		xcb_disconnect(_connection);

	return -1;
}

void destroyDisplayContext(DisplayContext _Context) noexcept {
	xcb_disconnect(_Context->connection);

	delete _Context;
}

struct DisplayWindow_T{
	xcb_window_t window;
	const xcb_screen_t* screen;
	uint16_t width;
	uint16_t height;
	int16_t x;
	int16_t y;
};

int createDisplayWindow(const DisplayContext _Context, const WindowCreateInfo* const pCreateInfo, DisplayWindow* const pWindow) noexcept {
	xcb_connection_t* const connection = _Context->connection;

	xcb_window_t _window{};

	do {
		const xcb_setup_t* setup = xcb_get_setup(connection);
		xcb_screen_iterator_t iterator = xcb_setup_roots_iterator(setup);

		xcb_screen_t* screen = iterator.data;

		_window = xcb_generate_id(connection);
		
		uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		uint32_t values[] = { screen->black_pixel, XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_POINTER_MOTION };
		
		xcb_void_cookie_t cookie = xcb_create_window_checked(connection, XCB_COPY_FROM_PARENT, _window, screen->root, pCreateInfo->x, pCreateInfo->y, pCreateInfo->width, pCreateInfo->height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask, values);

		if (xcb_generic_error_t* error = xcb_request_check(connection, cookie)) {
			std::free(error);
			break;
		}

		xcb_change_property(connection, XCB_PROP_MODE_REPLACE, _window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(pCreateInfo->title), pCreateInfo->title);
		xcb_change_property(connection, XCB_PROP_MODE_REPLACE, _window, _Context->wmProtocols, XCB_ATOM_ATOM, 32, 1, &_Context->wmDeleteWindow);

		if (!xcb_flush(connection))
			break;

		DisplayWindow const displayWindow = new(std::nothrow) DisplayWindow_T;

		if (!displayWindow)
			break;

		displayWindow->window = _window;
		displayWindow->screen = screen;
		displayWindow->width = pCreateInfo->width;
		displayWindow->height = pCreateInfo->height;
		displayWindow->x = pCreateInfo->x;
		displayWindow->y = pCreateInfo->y;

		*pWindow = displayWindow;

		return 0;

	} while (false);

	if (_window)
		xcb_destroy_window(connection, _window);

	return -1;
}

void destroyDisplayWindow(const DisplayContext _Context, DisplayWindow const _Window) noexcept {
	xcb_connection_t* const connection = _Context->connection;

	xcb_destroy_window(connection, _Window->window);
	xcb_flush(connection);
}

void raiseDisplayWindow(DisplayContext const _Context, DisplayWindow const _Window) noexcept {
	const xcb_screen_t* const screen = _Window->screen;

	xcb_client_message_event_t event{};
	event.response_type = XCB_CLIENT_MESSAGE;
	event.window = _Window->window;
	event.type = _Context->netActiveWindow;
	event.format = 32;
	event.data.data32[0] = 1;
	event.data.data32[1] = XCB_CURRENT_TIME;
	event.data.data32[2] = XCB_WINDOW_NONE;
	event.data.data32[3] = 0;
	event.data.data32[4] = 0;

	xcb_map_window(_Context->connection, _Window->window);
	xcb_send_event(_Context->connection, 0, screen->root, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY, reinterpret_cast<const char*>(&event));

	xcb_flush(_Context->connection);
}

int queryWindowGeometry(const DisplayContext _Context, DisplayWindow const _Window, WindowGeomentry* const pGeomentry) noexcept {
	xcb_connection_t* const connection = _Context->connection;

	const xcb_get_geometry_cookie_t cookie = xcb_get_geometry(connection, _Window->window);
	xcb_get_geometry_reply_t* const reply = xcb_get_geometry_reply(connection, cookie, nullptr);

	if (!reply)
		return -1;

	pGeomentry->x = reply->x;
	pGeomentry->y = reply->y;
	pGeomentry->width = reply->width;
	pGeomentry->height = reply->height;

	std::free(reply);

	return 0;
}

int setWindowGeometry(const DisplayContext _Context, DisplayWindow const _Window, const WindowGeomentry* const pGeometry) noexcept {
	uint32_t values[] = { 
		static_cast<uint32_t>(pGeometry->x),
		static_cast<uint32_t>(pGeometry->y),
		static_cast<uint32_t>(pGeometry->width), 
		static_cast<uint32_t>(pGeometry->height) 
	};
	
	xcb_connection_t* const connection = _Context->connection;

	xcb_configure_window(connection, _Window->window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
	
	if (!xcb_flush(connection))
		return -1;

	_Window->width = pGeometry->width;
	_Window->height = pGeometry->height;
	_Window->x = pGeometry->x;
	_Window->y = pGeometry->y;

	return 0;
}

void setWindowTitle(const DisplayContext _Context, DisplayWindow const _Window, const char* const _Title) noexcept {
	xcb_connection_t* const connection = _Context->connection;
	const xcb_window_t window = _Window->window;
	
	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(_Title), _Title);
	xcb_flush(connection);
}

float getWindowAspect(DisplayWindow _Window) noexcept {
	return _Window->width/(float)_Window->height;
}

void getVulkanSurfaceDependencyInfo(DisplayContext const _Context, DisplayWindow const _Window, VulkanSurfaceDependencyInfo* const pDependencyInfo) noexcept {
	pDependencyInfo->context = (void*)_Context->connection;
	pDependencyInfo->window = (uintptr_t)_Window->window;
}

enum WindowEventType : uint32_t {
	WINDOW_EVENT_TYPE_MOTION,
	WINDOW_EVENT_TYPE_WINDOW_CONFIGURE,
	WINDOW_EVENT_TYPE_FOCUS_GAINED,
	WINDOW_EVENT_TYPE_FOCUS_LOST,
	WINDOW_EVENT_TYPE_MINIMIZE,
	WINDOW_EVENT_TYPE_RESTORED,
	WINDOW_EVENT_TYPE_WINDOW_CLOSE
};

struct WindowEvent {
	xcb_window_t window;
	WindowEventType type;
	struct {
		uint16_t width;
		uint16_t height;
	} extent;
	struct {
		int16_t x;
		int16_t y;
	} position;
};

struct EventBuffer_T {
	uint32_t count;
	WindowEvent* event;
};

int createEventBuffer(const uint32_t capacity, EventBuffer* const pEventBuffer) noexcept {
	WindowEvent* _event = nullptr;

	do {
		_event = mem_allocateSizeRange<WindowEvent>(capacity);

		if (!_event)
			break;

		EventBuffer const buffer = new(std::nothrow) EventBuffer_T;

		if (!buffer)
			break;

		buffer->count = 0;
		buffer->event = _event;

		*pEventBuffer = buffer;

		return 0;

	} while (false);

	if (_event)
		mem_freeSizeRange<WindowEvent>(_event);

	return -1;
}

void destroyEventBuffer(EventBuffer const _EventBuffer) noexcept {
	mem_freeSizeRange<WindowEvent>(_EventBuffer->event);
	
	delete _EventBuffer;
}

static inline bool translateXCBEvent(DisplayContext const _Context, xcb_generic_event_t* const pXCBEvent, WindowEvent* const pEvent) noexcept {
	const uint8_t type = pXCBEvent->response_type & ~0x80;

	switch (type) {
	case XCB_MOTION_NOTIFY: {
		const xcb_motion_notify_event_t* const motion = reinterpret_cast<xcb_motion_notify_event_t*>(pXCBEvent);

		pEvent->window = motion->event;
		pEvent->type = WINDOW_EVENT_TYPE_MOTION;

		pEvent->position.x = motion->event_x;
		pEvent->position.y = motion->event_y;
	}

		return true;

	case XCB_CONFIGURE_NOTIFY: {
		const xcb_configure_notify_event_t* const configure = reinterpret_cast<xcb_configure_notify_event_t*>(pXCBEvent);

		pEvent->window = configure->window;
		pEvent->type = WINDOW_EVENT_TYPE_WINDOW_CONFIGURE;

		pEvent->extent = { configure->width, configure->height };
		pEvent->position = { configure->x, configure->y };
	}

		return true;

	case XCB_FOCUS_IN: {
		const xcb_focus_in_event_t* const focus = reinterpret_cast<xcb_focus_in_event_t*>(pXCBEvent);

		pEvent->window = focus->event;
		pEvent->type = WINDOW_EVENT_TYPE_FOCUS_GAINED;
	}

		return true;

	case XCB_FOCUS_OUT: {
		const xcb_focus_out_event_t* const focus = reinterpret_cast<xcb_focus_out_event_t*>(pXCBEvent);

		pEvent->window = focus->event;
		pEvent->type = WINDOW_EVENT_TYPE_FOCUS_LOST;
	}

		return true;

	case XCB_PROPERTY_NOTIFY: {
		const xcb_property_notify_event_t* const property = reinterpret_cast<xcb_property_notify_event_t*>(pXCBEvent);

		if (property->atom != _Context->wmState)
			break;

		xcb_get_property_cookie_t cookie = xcb_get_property(_Context->connection, 0, property->window, _Context->wmState, XCB_ATOM_ANY, 0, 2);

		xcb_get_property_reply_t* reply = xcb_get_property_reply(_Context->connection, cookie, nullptr);

		if (!reply)
			break;

		if (reply->format == 32 && reply->type == _Context->wmState && reply->value_len) {
			const uint32_t state = *static_cast<const uint32_t*>(xcb_get_property_value(reply));

			pEvent->window = property->window;

			if (state == XCB_ICCCM_WM_STATE_ICONIC)
				pEvent->type = WINDOW_EVENT_TYPE_MINIMIZE;
			else if (state == XCB_ICCCM_WM_STATE_NORMAL)
				pEvent->type = WINDOW_EVENT_TYPE_RESTORED;

			std::free(reply);
			return true;
		}

		std::free(reply);
	}

		break;

	case XCB_CLIENT_MESSAGE: {
		const xcb_client_message_event_t* const message = reinterpret_cast<xcb_client_message_event_t*>(pXCBEvent);

		if (message->type != _Context->wmProtocols)
			break;

		if (message->data.data32[0] != _Context->wmDeleteWindow)
			break;

		pEvent->window = message->window;
		pEvent->type = WINDOW_EVENT_TYPE_WINDOW_CLOSE;
	}

		return true;

	}

	return false;
}

bool pollWindowEvents(DisplayContext const _Context, EventBuffer const _EventBuffer) noexcept {
	xcb_connection_t* const connection = _Context->connection;

	WindowEvent* pEvent = _EventBuffer->event;
	const WindowEvent* const pEventEnd = mem_getSizeAllocationEnd(_EventBuffer->event);
	while (pEvent != pEventEnd) {
		xcb_generic_event_t* const event = xcb_poll_for_event(connection);

		if (!event)
			break;

		if (translateXCBEvent(_Context, event, pEvent))
			++pEvent;

		std::free(event);
	}

	_EventBuffer->count = static_cast<uint32_t>(pEvent - _EventBuffer->event);

	return _EventBuffer->count != 0;
}

bool waitWindowEvents(DisplayContext const _Context, EventBuffer const _EventBuffer) noexcept {
	xcb_generic_event_t* const event = xcb_wait_for_event(_Context->connection);
	
	if (!event)
		return false;

	if (translateXCBEvent(_Context, event, _EventBuffer->event + _EventBuffer->count))
		_EventBuffer->count++;

	std::free(event);
	return true;
}

void resolveWindowEvents(EventBuffer const _EventBuffer, DisplayWindow const _Window, WindowStateField* const pState, DisplayCursor* const pCursor) noexcept {
	const WindowEvent* const pEventEnd = _EventBuffer->event + _EventBuffer->count;

	const xcb_window_t window = _Window->window;

	WindowStateField state = 0;

	for (const WindowEvent* pEvent{ _EventBuffer->event }; pEvent != pEventEnd; ++pEvent) {
		if (pEvent->window != window)
			continue;

		switch (pEvent->type) {
		case WINDOW_EVENT_TYPE_MOTION:
			pCursor->x = pEvent->position.x;
			pCursor->y = pEvent->position.y;

			state |= WINDOW_STATE_POINTER_MOTION_BIT;

			break;

		case WINDOW_EVENT_TYPE_WINDOW_CONFIGURE:
			if (pEvent->extent.width != _Window->width || pEvent->extent.height != _Window->height) {
				state |= WINDOW_STATE_EXTENT_DIRTY_BIT;
				_Window->width = pEvent->extent.width;
				_Window->height = pEvent->extent.height;
			}
			
			if (pEvent->position.x != _Window->x || pEvent->position.y != _Window->y) {
				state |= WINDOW_STATE_POSITION_DIRTY_BIT;
				_Window->x = pEvent->position.x;
				_Window->y = pEvent->position.y;
			}
		
			break;
			
		case WINDOW_EVENT_TYPE_FOCUS_GAINED:
			state |= WINDOW_STATE_FOCUSED_BIT;

			break;

		case WINDOW_EVENT_TYPE_FOCUS_LOST:
			state &= ~WINDOW_STATE_FOCUSED_BIT;

			break;

		case WINDOW_EVENT_TYPE_MINIMIZE:
			state |= WINDOW_STATE_MINIMIZED_BIT;
			
			break;

		case WINDOW_EVENT_TYPE_RESTORED:
			state &= ~WINDOW_STATE_MINIMIZED_BIT;

			break;

		case WINDOW_EVENT_TYPE_WINDOW_CLOSE:
			state |= WINDOW_STATE_TERMINATION_IMMINENT_BIT;

		default:
			break;
		}
	}

	_EventBuffer->count = 0;

	*pState = state;
}

  #elif defined(WINDOW_X11)
    #include <X11/Xlib.h>
  #elif defined(WINDOW_WAYLAND)
    #include <wayland-client.h>
  #else
	#error "Window backend not specified"
  #endif
#endif
