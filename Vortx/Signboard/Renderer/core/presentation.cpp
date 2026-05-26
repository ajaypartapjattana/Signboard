#include "Signboard/Renderer/Internal/rndr_pAccess.h"

namespace rndr {

	crePresentation::crePresentation(const context& context, creResources& resources) noexcept
		:
		r_images(_pAccess::imageWrite(resources)),
		m_swapchainCreate(_pAccess::device(context), _pAccess::surface(context)),
		m_swpachainImageAllocate(_pAccess::device(context))
	{
		
	}

	void crePresentation::defaultSwapchainParameters() noexcept {
		m_swapchainCreate.imageFormat(VK_FORMAT_B8G8R8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR);
		m_swapchainCreate.imageCount(DEFAULT_SWAPCHAIN_IMAGE_COUNT);
		m_swapchainCreate.presentMode(VK_PRESENT_MODE_FIFO_KHR);
		m_swapchainCreate.carry_surface();
	}

	void crePresentation::validateSwapchain() noexcept {
		m_swapchainCreate.carry_surface();
	}

	void crePresentation::swapchainImageCount(uint32_t count) noexcept {
		m_swapchainCreate.imageCount(std::max(2u, count));
	}

	void crePresentation::constructSwapchain(const rhi::pcdWatchdog& watchdog) {
		m_swapchainCreate.recycle_swapchain(m_swapchain);

		watchdog.wait_device();

		VkResult result = m_swapchainCreate.publish(m_swapchain);
		if (result != VK_SUCCESS)
			throw std::runtime_error("FAILURE: swapchain_creation!");

		m_swpachainImageAllocate.target_swapchain(m_swapchain);

		uint32_t _siCt = m_swpachainImageAllocate.get_imageCount();

		for (uint32_t idx : swapchainImageHandles) {
			r_images.destroy(idx);
		}

		auto _ctor = [&](uint32_t i, rhi::pmvImage* image) {
			result = m_swpachainImageAllocate.publish(*image, i);
			if (result != VK_SUCCESS)
				throw std::runtime_error("FAILURE: swapchain_image_allocation!");
		};

		swapchainImageHandles = r_images.construct_many(_siCt, _ctor);
	}

	uint32_t crePresentation::availableImageCount() const noexcept {
		return static_cast<uint32_t>(swapchainImageHandles.size());
	}

}
