#include "commandPool.h"

namespace rhi {

	pmvCommandPool::pmvCommandPool() noexcept 
		: 
		m_commandPool(VK_NULL_HANDLE),
		m_queueFamilyIndex(0),
		_dvc(VK_NULL_HANDLE)
	{

	}

	pmvCommandPool::pmvCommandPool(pmvCommandPool&& other) noexcept 
		:
		m_commandPool(other.m_commandPool),
		m_queueFamilyIndex(other.m_queueFamilyIndex),
		_dvc(other._dvc)
	{
		other.m_commandPool = VK_NULL_HANDLE;
	}

	pmvCommandPool& pmvCommandPool::operator=(pmvCommandPool&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_commandPool)
			vkDestroyCommandPool(_dvc, m_commandPool, nullptr);

		m_commandPool = other.m_commandPool;
		m_queueFamilyIndex = other.m_queueFamilyIndex;
		_dvc = other._dvc;

		other.m_commandPool = VK_NULL_HANDLE;

		return *this;
	}

	pmvCommandPool::~pmvCommandPool() noexcept {
		if (m_commandPool)
			vkDestroyCommandPool(_dvc, m_commandPool, nullptr);
	}

	VkCommandPool pmvCommandPool::native_commandPool() const noexcept {
		return m_commandPool;
	}

}