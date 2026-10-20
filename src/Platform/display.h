#pragma once

#include <cstdint>

#include "input.h"

struct DisplayContext_T;
using DisplayContext = DisplayContext_T*;

int requestDisplayContext(DisplayContext* pContext) noexcept;
void destroyDisplayContext(DisplayContext context) noexcept;

struct DisplayWindow_T;
using DisplayWindow = DisplayWindow_T*;

struct WindowGeomentry {
	int16_t x;
	int16_t y;
	uint16_t width;
	uint16_t height;
};

using WindowCreateFlags = uint32_t;
enum WindowCreateFlagBit : WindowCreateFlags {
	WINDOW_CREATE_STATIC_GEOMETRY_BIT = 1u << 0,
	WINDOW_CREATE_FULLSCREEN_BIT = 1u << 0
};

struct WindowCreateInfo {
	WindowCreateFlags flags;
	WindowGeomentry geometry;
	const char* title;
	WindowInput* pInput;
};

int createDisplayWindow(DisplayContext context, const WindowCreateInfo* pCreateInfo, DisplayWindow* pWindow) noexcept;
void destroyDisplayWindow(DisplayWindow window) noexcept;

void raiseDisplayWindow(DisplayWindow window) noexcept;
void queryWindowGeometry(DisplayWindow window, WindowGeomentry* pGeomentry) noexcept;
int setWindowGeometry(DisplayWindow window, const WindowGeomentry* pGeometry) noexcept;
void setWindowTitle(DisplayWindow window, const char* title) noexcept;

float getWindowAspect(DisplayWindow window) noexcept;

void pollWindowEvents() noexcept;

using WindowNotification = uint32_t;
enum WindowNotificationBits : WindowNotification {
	WINDOW_NOTIFY_TERMINATION_BIT = 1u << 0,
	WINDOW_NOTIFY_FOCUS_GAIN_BIT = 1u << 1,
	WINDOW_NOTIFY_FOCUS_LOST_BIT = 1u << 2,
	WINDOW_NOTIFY_STATE_BIT = 1u << 3,
	WINDOW_NOTIFY_MOVE_BIT = 1u << 4,
	WINDOW_NOTIFY_INPUT_BIT = 1u << 5
};

WindowNotification queryWindowNotification(DisplayWindow window) noexcept;
void clearWindowNotification(DisplayWindow window) noexcept;

enum WindowState : uint32_t {
	WINDOW_STATE_UNDEFINED,
	WINDOW_STATE_MINIMIZED,
	WINDOW_STATE_WINDOWED,
	WINDOW_STATE_MAXIMIZED,
	WINDOW_STATE_FULLSCREEN
};

WindowState getWindowState(DisplayWindow window) noexcept;

void waitWindowEvents() noexcept;

struct VulkanSurfaceDependencyInfo {
	void* context;
	uintptr_t window;
};

void getVulkanSurfaceDependencyInfo(DisplayContext _Context, DisplayWindow _Window, VulkanSurfaceDependencyInfo* pDependencyInfo) noexcept;
