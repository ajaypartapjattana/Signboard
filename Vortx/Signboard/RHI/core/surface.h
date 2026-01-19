#pragma once

namespace rhi::procedure {
	class surface_builder;
}

namespace rhi::core {

	struct surface_vkAccess;

	class surface {
	public:
		surface(const surface&) = delete;
		surface& operator=(const surface&) = delete;

		surface(surface&&) noexcept;
		surface& operator=(surface&&) noexcept;

		~surface() noexcept;

		const void* native_surface() const noexcept;

	private:
		friend class rhi::procedure::surface_builder;
		friend struct surface_vkAccess;

		surface() = default;

		void* m_instance = nullptr;

		void* m_surface = nullptr;
		void* m_allocator = nullptr;

	};

}