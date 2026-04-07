#include "buffer.h"

namespace rhi {

	pmvBuffer::pmvBuffer() noexcept
		:
		m_buffer(VK_NULL_HANDLE),
		allocation(VK_NULL_HANDLE),
		_mpd(nullptr),
		_is_host_coherent(false),
		_allctr(VK_NULL_HANDLE)
	{

	}

	pmvBuffer::pmvBuffer(pmvBuffer&& other) noexcept 
		:
		m_buffer(other.m_buffer),
		allocation(other.allocation),
		_mpd(other._mpd),
		_is_host_coherent(other._is_host_coherent),
		_allctr(other._allctr)
	{
		other.m_buffer = VK_NULL_HANDLE;
	}

	pmvBuffer& pmvBuffer::operator=(pmvBuffer&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_buffer)
			vmaDestroyBuffer(_allctr, m_buffer, allocation);

		m_buffer = other.m_buffer;
		allocation = other.allocation;
		_mpd = other._mpd;
		_is_host_coherent = other._is_host_coherent;
		_allctr = other._allctr;

		other.m_buffer = VK_NULL_HANDLE;

		return *this;
	}

	pmvBuffer::~pmvBuffer() noexcept {
		if (m_buffer)
			vmaDestroyBuffer(_allctr, m_buffer, allocation);
	}

	void* pmvBuffer::native_mapped() const {
		return _mpd;
	}

	void pmvBuffer::flush(VkDeviceSize begin, VkDeviceSize end) const {
		if (!_is_host_coherent && begin < end) {
			vmaFlushAllocation(_allctr, allocation, begin, end - begin);
		}
	}


}