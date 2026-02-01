#pragma once

#include <GLFW/glfw3.h>

template<class user_t>
class WindowEventProxy{
public:
	void attachWindow(GLFWwindow* window);

	void bindUser(user_t* user);

	void onFrameBufferResize(int width, int height);
	void onFileDrop(int count, const char** paths);

private:
	GLFWwindow* m_window = nullptr;
	user_t* m_user = nullptr;
};