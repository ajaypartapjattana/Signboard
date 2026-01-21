#pragma once

namespace rhi::core {
	class swapchain;
	class device;
	class surface;
}

#include <cstdint>

namespace rhi::procedure {

	class swapchain_builder {
	public:
		swapchain_builder(const rhi::core::device& device, const rhi::core::surface& surface);

		swapchain_builder& prefer_format();
		swapchain_builder& prefer_presentMode();

		swapchain_builder& set_extent(uint32_t w, uint32_t h);
		swapchain_builder& set_imageCount(uint32_t count);

		swapchain_builder* allow_tearing(bool);

		rhi::core::swapchain build();
		swapchain_builder& update_swapchain(const rhi::core::swapchain&);

	private:
		void query_support();

	private:
		struct Impl;
		Impl* impl;

	};

}