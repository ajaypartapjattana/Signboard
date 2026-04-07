#pragma once

#include "Common/VulkanFwd.h"
#include "Signboard/RHI/common/BufferTypes.h"

class VulkanDevice;
class VulkanCommandBuffer;

class VulkanBuffer {
public:
	VulkanBuffer(VulkanDevice& device, const BufferDesc& desc);

	VulkanBuffer(const VulkanBuffer&) = delete;
	VulkanBuffer& operator=(const VulkanBuffer&) = delete;

	VulkanBuffer(VulkanBuffer&& other) noexcept;
	VulkanBuffer& operator=(VulkanBuffer&& other) noexcept;

	void destroy();
	~VulkanBuffer();

	void* map();
	void unmap();
	void recordUploads(const void* data, uint64_t size, uint64_t offset = 0);

	void copyFrom(VulkanCommandBuffer& CMDGraphics, const VulkanBuffer& src, uint64_t size, uint64_t srcOffset = 0, uint64_t dstOffset = 0);

	VkBuffer getHandle() const { return buffer; }
	uint64_t getSize() const { return size; }

private:
	void allocateMesh(BufferUsageFlags usage);
	void allocateMemory(MemoryPropertyFlags memoryProperty);
	void bindMemory();

private:
	VulkanDevice& device;

	VkBuffer buffer;
	VkDeviceMemory memory;

	void* _mpd = nullptr;
	uint64_t size;

	MemoryPropertyFlags memoryProperties;
	BufferUsageFlags usage;
};
