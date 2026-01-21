#include "context.h"

#include <GLFW/glfw3.h>
#include <stdexcept>

namespace window::core {

	context::context() {
		if (!glfwInit())
			throw std::runtime_error("GLFW initialization failed!");
	}

	context::~context() {
		glfwTerminate();
	}

}