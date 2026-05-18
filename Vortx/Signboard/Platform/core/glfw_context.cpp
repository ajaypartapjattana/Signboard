#include "Signboard/Platform/internal/plf_pAccess.h"

#include <GLFW/glfw3.h>
#include <stdexcept>

namespace plf {

	context::context() {
		if (!glfwInit())
			throw std::runtime_error("FAILURE: glfw_unintialized!");
	}

	context::~context() {
		glfwTerminate();
	}

	const char** context::native_extensions(uint32_t& count) noexcept {
		const char** glfwExts = glfwGetRequiredInstanceExtensions(&count);

		return glfwExts;
	}

}