#pragma once

#include "Signboard/Core/Interfaces/startupConfigType.h"

#include <string>
#include <iostream>
#include <filesystem>

struct GLFWwindow;

class WindowSurface {
public:
    WindowSurface(const WindowInfo& info);
	~WindowSurface();

    GLFWwindow* getWindowHandle() const { return window; }

private:
    GLFWwindow* window;
};
