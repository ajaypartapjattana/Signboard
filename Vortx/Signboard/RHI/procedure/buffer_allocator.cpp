#include "buffer_allocator.h"

#include "Signboard/RHI/primitive/buffer.h"
#include "Signboard/RHI/core/allocator_vma.h"

#include <stdexcept>

namespace rhi::procedure {

	buffer_allocator::buffer_allocator(const rhi::core::allocator& allocator)
		: m_allocator(rhi::core::allocator_vmaAccess::get(allocator))
	{
		reset_allocatorState();
	}

	buffer_allocator& buffer_allocator::set_usage_STAGING() {
		set_memory(VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
		set_usage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		return *this;
	}

	buffer_allocator& buffer_allocator::set_usage_VERTEX() {
		set_usage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		return *this;
	}

	buffer_allocator& buffer_allocator::set_usage_INDEX() {
		set_usage(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		return *this;
	}

	buffer_allocator& buffer_allocator::set_usage_STORAGE() {
		set_usage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
		return *this;
	}

	buffer_allocator& buffer_allocator::set_usage_UNIFORM() {
		set_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		return *this;
	}

	buffer_allocator& buffer_allocator::set_usage_TRANSFER_DST() {
		set_usage(VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		return *this;
	}

	buffer_allocator& buffer_allocator::set_size(VkDeviceSize size) {
		final_size = size;
		return *this;
	}

	rhi::primitive::buffer buffer_allocator::allocate() {
		if (final_size == 0)
			throw std::logic_error("BUFFER_ALLOCATOR: size_invalid!");

		if (final_usage == 0)
			throw std::logic_error("BUFFER_ALLOCATOR: usage_invalid!");

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = final_size;
		bufferInfo.usage = final_usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = final_memory;

		VkBuffer vk_buffer = VK_NULL_HANDLE;
		VmaAllocation vma_allocation = VK_NULL_HANDLE;
		if (vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo, &vk_buffer, &vma_allocation, nullptr) != VK_SUCCESS)
			throw std::runtime_error("FAILED: buffer_allocation!");

		rhi::primitive::buffer l_buffer;
		l_buffer.m_buffer = vk_buffer;
		l_buffer.m_allocation = vma_allocation;
		l_buffer.m_allocator = m_allocator;

		reset_allocatorState();

		return l_buffer;
	}

	void buffer_allocator::set_memory(VmaMemoryUsage usage) {
		final_memory = usage;
	}

	void buffer_allocator::set_usage(VkBufferUsageFlags usage) {
		final_usage |= usage;
	}

	void buffer_allocator::reset_allocatorState() {
		final_usage = 0;
		final_memory = VMA_MEMORY_USAGE_AUTO;
		final_size = 0;
	}

}