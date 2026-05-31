#pragma once

#include <vulkan/vulkan.h>
#include <utility>

namespace rhi {

	class swapchain {
	private:
		VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
		VkDevice r_device = VK_NULL_HANDLE;

	public:
		swapchain() = default;
		swapchain(VkDevice device, VkSwapchainCreateInfoKHR* pCreateInfo) {

		}
		swapchain(const swapchain&) = delete;
		swapchain(swapchain&& other) noexcept 
			:
			m_swapchain(std::exchange(other.m_swapchain, VK_NULL_HANDLE)),
			r_device(std::exchange(other.r_device, VK_NULL_HANDLE))
		{

		}

		swapchain& operator=(const swapchain&) = delete;
		swapchain& operator=(swapchain&& other) noexcept {
			if (this == &other)
				return *this;

			reset();

			m_swapchain = std::exchange(other.m_swapchain, VK_NULL_HANDLE);
			r_device = std::exchange(other.r_device, VK_NULL_HANDLE);

			return *this;
		}

		~swapchain() noexcept {
			reset();
		}

		operator VkSwapchainKHR() const noexcept {
			return m_swapchain;
		}

		VkResult acquireImage(VkSemaphore semaphore, uint32_t* imageIndex) const noexcept {
			return vkAcquireNextImageKHR(r_device, m_swapchain, UINT64_MAX, semaphore, VK_NULL_HANDLE, imageIndex);
		}

		VkResult create(VkDevice device, VkSwapchainCreateInfoKHR* pCreateInfo) noexcept;
		void reset() noexcept;

	};

}