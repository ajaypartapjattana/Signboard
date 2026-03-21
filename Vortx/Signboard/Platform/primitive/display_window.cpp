#include "display_window.h"

#include <stdexcept>

namespace platform::primitive {

	displayWindow::displayWindow(const createInfo& createInfo) 
		:
		m_window(nullptr)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		GLFWmonitor* moniter = nullptr;

		createInfo::Extent2D a_extent = createInfo.extent;

		if (createInfo.fullscreen) {
			moniter = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(moniter);

			a_extent.width = mode->width;
			a_extent.height = mode->height;
		}

		m_window = glfwCreateWindow(a_extent.width, a_extent.height, (createInfo.title).c_str(), moniter, nullptr);

		if (!m_window)
			throw std::runtime_error("failed to create display window!");
	}

	displayWindow::displayWindow(displayWindow&& other) noexcept 
		:
		m_window(other.m_window)
	{
		other.m_window = nullptr;
	}

	displayWindow& displayWindow::operator=(displayWindow&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_window)
			glfwDestroyWindow(m_window);

		m_window = other.m_window;
		other.m_window = nullptr;

		return *this;
	}

	displayWindow::~displayWindow() noexcept {
		if (m_window) {
			glfwDestroyWindow(m_window);
		}
	}

	const GLFWwindow* displayWindow::native_window() const noexcept {
		return m_window;
	}

}