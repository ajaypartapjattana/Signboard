#pragma once

#include <vector>
#include <cstdint>

namespace rhi::core {
	class instance;
	class device;
	class surface;
}

namespace rhi::procedure {

	class physical_device_selector;

	class device_builder {
	public:
		device_builder(const rhi::core::instance& instance);

		device_builder(const device_builder&) = delete;
		device_builder& operator=(const device_builder&) = delete;

		device_builder& require_graphicsQueue();
		device_builder& require_computeQueue();
		device_builder& require_transferQueue();
		device_builder& require_presentQueue(const rhi::core::surface& s);

		device_builder& enable_anisotropy();

		rhi::core::device build();


	private:
		friend class physical_device_selector;

		struct Impl;
		Impl* impl;

	};

}