#pragma once

namespace rhi::core {
	class allocator;
}

namespace rhi::primitive {
	class buffer;
}

#include <vma.h>

namespace rhi::procedure {

	class buffer_allocator {
	public:
		buffer_allocator(const rhi::core::allocator& allocator);

		buffer_allocator(const buffer_allocator&) = delete;
		buffer_allocator& operator=(const buffer_allocator&) = delete;

		buffer_allocator& set_usage_STAGING();

		buffer_allocator& set_usage_VERTEX();
		buffer_allocator& set_usage_INDEX();
		buffer_allocator& set_usage_STORAGE();
		buffer_allocator& set_usage_UNIFORM();
		buffer_allocator& set_usage_TRANSFER_DST();

		buffer_allocator& set_size(VkDeviceSize size);

		[[nodiscard]] rhi::primitive::buffer allocate();

	private:
		void set_memory(VmaMemoryUsage usage);
		void set_usage(VkBufferUsageFlags usage);

		void reset_allocatorState();

	private:
		VmaAllocator m_allocator = VK_NULL_HANDLE;

		VkBufferUsageFlags final_usage{};

		VmaMemoryUsage final_memory{};

		VkDeviceSize final_size{};

	};

}