#pragma once

#include "Signboard/RHI/rhi.h"

namespace rndr {

	struct _pAccess;

	class context {
	private:
		friend struct _pAccess;

		rhi::instance instance;
		std::vector<rhi::physicalDevice> physicalDevices;

		rhi::surface surface;

		rhi::device device;
		rhi::allocator allocator;

	public:
		context();

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		float createDevice(GLFWwindow* window, uint32_t physicalDeviceIndex);

	};

}
