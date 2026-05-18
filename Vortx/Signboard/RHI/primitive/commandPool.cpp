#include "commandPool.h"

namespace rhi {

	pmvCommandPool::pmvCommandPool() noexcept 
		: 
		m_commandPool(VK_NULL_HANDLE),
		family(0),
		r_device(VK_NULL_HANDLE)
	{

	}

	pmvCommandPool::pmvCommandPool(const pmvCommandPool& other) noexcept 
		:
		m_commandPool(other.m_commandPool),
		family(other.family),
		r_device(VK_NULL_HANDLE)
	{

	}

	pmvCommandPool& pmvCommandPool::operator=(const pmvCommandPool& other) noexcept {
		if (this == &other)
			return *this;

		if (r_device)
			vkDestroyCommandPool(r_device, m_commandPool, nullptr);

		m_commandPool = other.m_commandPool;
		family = other.family;
		r_device = VK_NULL_HANDLE;

		return *this;
	}

	pmvCommandPool::pmvCommandPool(pmvCommandPool&& other) noexcept 
		:
		m_commandPool(other.m_commandPool),
		family(other.family),
		r_device(other.r_device)
	{
		other.r_device = VK_NULL_HANDLE;
	}

	pmvCommandPool& pmvCommandPool::operator=(pmvCommandPool&& other) noexcept {
		if (this == &other)
			return *this;

		if (r_device)
			vkDestroyCommandPool(r_device, m_commandPool, nullptr);

		m_commandPool = other.m_commandPool;
		family = other.family;
		r_device = other.r_device;

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	pmvCommandPool::~pmvCommandPool() noexcept {
		if (r_device)
			vkDestroyCommandPool(r_device, m_commandPool, nullptr);
	}

	void pmvCommandPool::reset() noexcept {
		if (r_device) {
			vkDestroyCommandPool(r_device, m_commandPool, nullptr);
		}

		r_device = VK_NULL_HANDLE;
	}

}