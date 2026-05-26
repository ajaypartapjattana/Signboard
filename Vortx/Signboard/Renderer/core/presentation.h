#pragma once

#include "Signboard/Core/core.h"
#include "Signboard/RHI/rhi.h"

constexpr uint32_t DEFAULT_SWAPCHAIN_IMAGE_COUNT = 2;

namespace rndr {

	class context;
	class creResources;

	struct _pAccess;

	class crePresentation {
	public:
		crePresentation(const context& context, creResources& resources) noexcept;

		crePresentation(const crePresentation&) = delete;
		crePresentation& operator=(const crePresentation&) = delete;

		void swapchainImageCount(uint32_t count) noexcept;
		void constructSwapchain(const rhi::pcdWatchdog& watchdog);

		void validateSwapchain() noexcept;

		uint32_t availableImageCount() const noexcept;

	private:
		void defaultSwapchainParameters() noexcept;

	private:
		friend struct _pAccess;

		sgb::vltWrite<rhi::pmvImage> r_images;

		rhi::pcdSwapchainCreate m_swapchainCreate;
		rhi::pcdSwapchainImageAllocate m_swpachainImageAllocate;

		rhi::creSwapchain m_swapchain;
		std::vector<uint32_t> swapchainImageHandles;

	};

}

