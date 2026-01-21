#pragma once

namespace rhi::procedure {
	class swapchain_builder;
}

namespace rhi::core {

	struct swapchain_vkAccess;

	class swapchain {
	public:
		swapchain(const swapchain&) = delete;
		swapchain& operator=(const swapchain&) = delete;

		swapchain(swapchain&&) noexcept;
		swapchain& operator=(swapchain&&) noexcept;

		~swapchain() noexcept;

		const void* native_swapchain() const noexcept;

	private:
		friend class rhi::procedure::swapchain_builder;
		friend struct swapchain_vkAccess;

		swapchain() = default;

		void* m_swapchain;
		
		void* m_device;
		void* m_allocator;

	};

}