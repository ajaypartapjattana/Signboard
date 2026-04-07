#include "VulkanBuffer.h"

#include "TypeMap/VulkanBufferTypeMap.h"

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"

VulkanBuffer::VulkanBuffer(VulkanDevice& device, const BufferDesc& desc)
	: device(device)
{
	size = desc.size;
	memoryProperties = desc.memoryFlags;
	usage = desc.usageFlags;

	allocateMesh(usage);
	allocateMemory(memoryProperties);
	bindMemory();
}

VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept
	: device(other.device), buffer(other.buffer), memory(other.memory), m_mapped(other.m_mapped), size(other.size), memoryProperties(other.memoryProperties), usage(other.usage)
{
	other.buffer = VK_NULL_HANDLE;
	other.memory = VK_NULL_HANDLE;
	other.m_mapped = nullptr;
}

VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& other) noexcept {
	if (this == &other)
		return *this;

	destroy();
	buffer = other.buffer;
	memory = other.memory;
	m_mapped = other.m_mapped;
	size = other.size;
	memoryProperties = other.memoryProperties;
	usage = other.usage;

	other.buffer = VK_NULL_HANDLE;
	other.memory = VK_NULL_HANDLE;
	other.m_mapped = nullptr;
}

VulkanBuffer::~VulkanBuffer() {
	destroy();
}

void VulkanBuffer::destroy() {
	VkDevice vkDevice = device.getDevice();

	if (m_mapped)
		unmap();

	vkDestroyBuffer(vkDevice, buffer, nullptr);
	vkFreeMemory(vkDevice, memory, nullptr);
}

void* VulkanBuffer::map() {
	if (memoryProperties.has(MemoryProperty::HostVisible)) {
		if (!m_mapped) {
			vkMapMemory(device.getDevice(), memory, 0, size, 0, &m_mapped);
		}
		return m_mapped;
	}
	throw std::runtime_error("buffer memory is not visible to host!");
}

void VulkanBuffer::unmap(){
	if (m_mapped) {
		vkUnmapMemory(device.getDevice(), memory);
		m_mapped = nullptr;
	}
}

void VulkanBuffer::recordUploads(const void* data, uint64_t dataSize, uint64_t offset = 0){
	if (!memoryProperties.has(MemoryProperty::HostVisible))
		throw std::runtime_error("buffer memory is not visible to host!");

	if (offset + dataSize > size)
		throw std::runtime_error("data size exceeds buffer capability!");

	void* dst = map();
	std::memcpy(static_cast<uint8_t*>(dst) + offset, data, dataSize);

	if (!memoryProperties.has(MemoryProperty::HostCoherent)) {
		VkMappedMemoryRange ranges{};
		ranges.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		ranges.memory = memory;
		ranges.offset = offset;
		ranges.size = dataSize;

		vkFlushMappedMemoryRanges(device.getDevice(), 1, &ranges);
	}
}

void VulkanBuffer::copyFrom(VulkanCommandBuffer& CMDGraphics, const VulkanBuffer& src, uint64_t copySize, uint64_t srcOffset = 0, uint64_t dstOffset = 0){
	if (copySize > size)
		throw std::runtime_error("data size exceeds buffer capability!");

	VkBufferCopy region{};
	region.srcOffset = srcOffset;
	region.dstOffset = dstOffset;
	region.size = copySize;

	vkCmdCopyBuffer(CMDGraphics.getHandle(), src.buffer, buffer, 1, &region);
}


void VulkanBuffer::allocateMesh(BufferUsageFlags usage) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = toVkBufferUsageFlags(usage);
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device.getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create a vulkan buffer!");
	}
}

void VulkanBuffer::allocateMemory(MemoryPropertyFlags memoryProperties) {
	VkDevice vkDevice = device.getDevice();

	VkMemoryRequirements memReq{};
	vkGetBufferMemoryRequirements(vkDevice, buffer, &memReq);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = device.findMemoryType(memReq.memoryTypeBits, memoryProperties);

	if (vkAllocateMemory(vkDevice, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate memory to a buffer");
	}
}

void VulkanBuffer::bindMemory() {
	vkBindBufferMemory(device.getDevice(), buffer, memory, 0);
}