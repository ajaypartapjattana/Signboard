#include "commandBuffer.h"

namespace rhi::primitive {

	commandBuffer::commandBuffer() noexcept 
		: m_commandBuffer(VK_NULL_HANDLE), m_commandPool(VK_NULL_HANDLE)
	{

	}

	commandBuffer::commandBuffer(commandBuffer&& other) noexcept {
		m_commandBuffer = other.m_commandBuffer;
		m_commandPool = other.m_commandPool;

		other.m_commandBuffer = VK_NULL_HANDLE;
		other.m_commandPool = VK_NULL_HANDLE;
	}

	commandBuffer& commandBuffer::operator=(commandBuffer&& other) noexcept {
		if (this == &other)
			return *this;

		m_commandBuffer = other.m_commandBuffer;
		m_commandPool = other.m_commandPool;

		other.m_commandBuffer = VK_NULL_HANDLE;
		other.m_commandPool = VK_NULL_HANDLE;

		return *this;
	}

	commandBuffer::~commandBuffer() noexcept {

	}

	VkCommandBuffer commandBuffer::native_commandBuffer() const noexcept {
		return m_commandBuffer;
	}

}