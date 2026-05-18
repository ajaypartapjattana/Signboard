#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	struct _pAccess;

	class pmvSemaphore;
	class pcdSwapchainCreate;

	class creSwapchain {
	public:
		creSwapchain() noexcept;

		creSwapchain(const creSwapchain&) = delete;
		creSwapchain& operator=(const creSwapchain&) = delete;

		creSwapchain(creSwapchain&&) noexcept;
		creSwapchain& operator=(creSwapchain&&) noexcept;

		~creSwapchain() noexcept;

		VkResult acquireImage(const pmvSemaphore& semaphore, uint32_t* imageIndex) const noexcept;

		void reset() noexcept;

		VkFormat native_format() const noexcept;
		VkExtent2D native_extent() const noexcept;

	private:
		friend class pcdSwapchainCreate;
		friend class pcdSwapchainImageAllocate;
		friend struct _pAccess;

		VkSwapchainKHR m_swapchain;
		
		VkExtent2D extent;
		uint32_t arrayLayers;
		VkFormat format;

		VkDevice r_device;

	};

}