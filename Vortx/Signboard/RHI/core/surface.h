#pragma once

namespace rhi::procedure {
	class surface_creator;
}

#include <vulkan/vulkan.h>

namespace rhi::core {

	struct surface_vkAccess;

	class surface {
	public:
		surface(const surface&) = delete;
		surface& operator=(const surface&) = delete;

		surface(surface&&) noexcept;
		surface& operator=(surface&&) noexcept;

		~surface() noexcept;

		const VkSurfaceKHR* native_surface() const noexcept;

	private:
		friend class rhi::procedure::surface_creator;
		friend struct surface_vkAccess;

		surface() = default;

		VkInstance m_instance;

		VkSurfaceKHR m_surface;
		VkAllocationCallbacks* m_allocator = nullptr;

	};

}