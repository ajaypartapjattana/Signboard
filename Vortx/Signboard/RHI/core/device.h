#pragma once

#include <cstdint>
#include <vector>

namespace rhi::procedure {
	class device_builder;
}

namespace rhi::core {

	struct device_vkAccess;

	class device {
	public:
		device(const device&) = delete;
		device& operator=(const device&) = delete;

		device(device&&) noexcept;
		device& operator=(device&&) noexcept;

		~device() noexcept;

		const void* native_device() const noexcept;

	private:
		friend class rhi::procedure::device_builder;
		friend struct device_vkAccess;

		device() = default;

		void* m_device;
		void* m_physical;

		struct queue_entry {
			void* queue;
			uint32_t family;
			uint32_t capabilities;
			bool present_supported;
		};
		std::vector<queue_entry> m_queues;

		void* m_allocator;

	};

}