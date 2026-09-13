#pragma once

#include <cstdint>

typedef uint32_t Flags;

using WindowCreateFlags = Flags;
enum WindowCreateFlagBit : WindowCreateFlags {
	WINDOW_CREATE_RESIZABLE_BIT = 1u << 0,
	WINDOW_CREATE_FULLSCREEN_BIT = 1u << 0
};

struct WindowCreateInfo {
	WindowCreateFlags flags;
	uint16_t width;
	uint16_t height;
	int16_t x;
	int16_t y;
	const char* title;
};

struct WindowGeomentry {
	int16_t x;
	int16_t y;
	uint16_t width;
	uint16_t height;
};

struct DisplayContext_T;
using DisplayContext = DisplayContext_T*;

int requestDisplayContext(DisplayContext* const pContext) noexcept;
void destroyDisplayContext(DisplayContext const _Context) noexcept;

struct DisplayWindow_T;
using DisplayWindow = DisplayWindow_T*;

int createDisplayWindow(DisplayContext _Context, const WindowCreateInfo* pCreateInfo, DisplayWindow* pWindow) noexcept;
void destroyDisplayWindow(DisplayContext _Context, DisplayWindow const _Window) noexcept;

void raiseDisplayWindow(DisplayContext _Context, DisplayWindow _Window) noexcept;
int queryWindowGeometry(DisplayContext _Context, DisplayWindow _Window, WindowGeomentry* pGeomentry) noexcept;
int setWindowGeometry(DisplayContext _Context, DisplayWindow _Window, const WindowGeomentry* pGeometry) noexcept;
void setWindowTitle(DisplayContext _Context, DisplayWindow _Window, const char* _Title) noexcept;

float getWindowAspect(DisplayWindow _Window) noexcept;

using WindowStateFlags = uint32_t;
enum WindowStateFlagBit : WindowStateFlags {
	WINDOW_STATE_TERMINATION_IMMINENT_BIT = 1u << 0,
	WINDOW_STATE_MINIMIZED_BIT = 1u << 1,
	WINDOW_EVENT_FOCUSED_BIT = 1u << 2,
	WINDOW_STATE_EXTENT_DIRTY_BIT = 1u << 3,
	WINDOW_STATE_POSITION_DIRTY_BIT = 1u << 4
};

struct EventBufferCreateInfo {
	uint32_t size;
};

struct EventBuffer_T;
using EventBuffer = EventBuffer_T*;

int createEventBuffer(const EventBufferCreateInfo* pCreateInfo, EventBuffer* pEventBuffer) noexcept;
void destroyEventBuffer(EventBuffer _EventBuffer) noexcept;

bool pollWindowEvents(DisplayContext _Context, EventBuffer _EventBuffer) noexcept;
bool waitWindowEvents(DisplayContext _Context, EventBuffer _EventBuffer) noexcept;
void resolveWindowEvents(EventBuffer _EventBuffer, DisplayWindow _Window, WindowStateFlags* pState) noexcept;

struct VulkanSurfaceDependencyInfo {
	void* context;
	uintptr_t window;
};

void getVulkanSurfaceDependencyInfo(DisplayContext _Context, DisplayWindow _Window, VulkanSurfaceDependencyInfo* pDependencyInfo) noexcept;
