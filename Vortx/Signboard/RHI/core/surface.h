#pragma once

namespace platform::primitive {
	class displayWindow;
}

#include <vulkan/vulkan.h>

namespace rhi::core {

	class instance;

	struct surface_vkAccess;

	class surface {
	public:
		surface(const platform::primitive::displayWindow& window, const instance& instance);

		surface(const surface&) = delete;
		surface& operator=(const surface&) = delete;

		surface(surface&&) noexcept;
		surface& operator=(surface&&) noexcept;

		~surface() noexcept;

		const VkSurfaceKHR* native_surface() const noexcept;

	private:
		friend struct surface_vkAccess;

		VkSurfaceKHR m_surface = VK_NULL_HANDLE;

		VkInstance m_instance = VK_NULL_HANDLE;

	};

}