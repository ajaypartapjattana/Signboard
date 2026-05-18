#include "commandBuffer.h"

namespace rhi {

	pmvCommandBuffer::pmvCommandBuffer() noexcept 
		:
		m_commandBuffer(VK_NULL_HANDLE), 
		r_commandPool(VK_NULL_HANDLE)
	{

	}

	pmvCommandBuffer::pmvCommandBuffer(pmvCommandBuffer&& other) noexcept 
		:
		m_commandBuffer(other.m_commandBuffer),
		r_commandPool(other.r_commandPool)
	{
		other.m_commandBuffer = VK_NULL_HANDLE;
	}

	pmvCommandBuffer& pmvCommandBuffer::operator=(pmvCommandBuffer&& other) noexcept {
		if (this == &other)
			return *this;

		m_commandBuffer = other.m_commandBuffer;
		r_commandPool = other.r_commandPool;

		other.m_commandBuffer = VK_NULL_HANDLE;

		return *this;
	}

	pmvCommandBuffer::~pmvCommandBuffer() noexcept {

	}

}