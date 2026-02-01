#include "WindowEventProxy.h"

template <class user_t>
void WindowEventProxy<user_t>::attachWindow(GLFWwindow* window)
{
    this->m_window = window;
	glfwSetWindowUserPointer(window, this);

	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetDropCallback(window, fileDropCallback);
}

template <class user_t>
void WindowEventProxy<user_t>::bindUser(user_t* user) {
    m_user = user;
}

template <class user_t>
void WindowEventProxy<user_t>::onFrameBufferResize(int width, int height) {
    if (m_user) {

    }
}

template <class user_t>
void WindowEventProxy<user_t>::onFileDrop(int count, const char** paths) {
    if (m_user) {

    }
}

template <class user_t>
void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto* windowEvent = static_cast<WindowEventProxy<user_t>*>(glfwGetWindowUserPointer(window));
    windowEvent->onFrameBufferResize(width, height);
}

template <class user_t>
void fileDropCallback(GLFWwindow* window, int count, const char** paths) {
    auto* windowEvent = static_cast<WindowEventProxy<user_t>*>(glfwGetWindowUserPointer(window));
    windowEvent->onFileDrop(count, paths);
}