#include "windowSurface.h"

#include <GLFW/glfw3.h>

WindowSurface::WindowSurface(
    const WindowInfo& info) 
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(info.width, info.height, info.title.c_str(), nullptr, nullptr);
}

WindowSurface::~WindowSurface() 
{
    glfwDestroyWindow(window);
    glfwTerminate();
}