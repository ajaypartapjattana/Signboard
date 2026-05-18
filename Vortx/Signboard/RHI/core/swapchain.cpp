#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	creSwapchain::creSwapchain() noexcept
		: 
		m_swapchain(VK_NULL_HANDLE),
		extent({ 0, 0 }),
		arrayLayers(0),
		format(VK_FORMAT_UNDEFINED),

		r_device(VK_NULL_HANDLE)
	{

	}

	creSwapchain::creSwapchain(creSwapchain&& other) noexcept
		:
		m_swapchain(other.m_swapchain),
		extent(other.extent),
		arrayLayers(other.arrayLayers),
		format(other.format),

		r_device(other.r_device)
	{
		other.r_device = VK_NULL_HANDLE;
	}

	creSwapchain& creSwapchain::operator=(creSwapchain&& other) noexcept {
		if (this == &other)
			return *this;

		if (r_device)
			vkDestroySwapchainKHR(r_device, m_swapchain, nullptr);

		m_swapchain = other.m_swapchain;
		extent = other.extent;
		format = other.format;
		r_device = other.r_device;

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	creSwapchain::~creSwapchain() noexcept {
		if (r_device)
			vkDestroySwapchainKHR(r_device, m_swapchain, nullptr);
	}

	VkResult creSwapchain::acquireImage(const pmvSemaphore& semaphore, uint32_t* imageIndex) const noexcept {
		return vkAcquireNextImageKHR(r_device, m_swapchain, UINT64_MAX, _pAccess::extract(semaphore), VK_NULL_HANDLE, imageIndex);
	}

	void creSwapchain::reset() noexcept {
		if (r_device) {
			vkDestroySwapchainKHR(r_device, m_swapchain, nullptr);
			r_device = VK_NULL_HANDLE;
		}

		m_swapchain = VK_NULL_HANDLE;
	}

	VkFormat creSwapchain::native_format() const noexcept {
		return format;
	}

	VkExtent2D creSwapchain::native_extent() const noexcept {
		return extent;
	}

}