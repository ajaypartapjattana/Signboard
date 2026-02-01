#include "context.h"

#include <GLFW/glfw3.h>
#include <stdexcept>

namespace platform::core {

	context::context() {
		if (!glfwInit())
			throw std::runtime_error("FAILURE: glfw_unintialized!");
	}

	context::~context() {
		glfwTerminate();
	}

	const char** context::native_extensions(uint32_t& count) {
		const char** glfwExts = glfwGetRequiredInstanceExtensions(&count);

		if (!glfwExts || count == 0)
			throw std::runtime_error("FAILURE: null_glfwRequiredExts!");

		return glfwExts;
	}

}