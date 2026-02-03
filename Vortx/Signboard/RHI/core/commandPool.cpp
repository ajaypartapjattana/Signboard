#include "commandPool.h"

namespace rhi::core {

	commandPool::commandPool() noexcept 
		: m_commandPool(VK_NULL_HANDLE), m_device(VK_NULL_HANDLE)
	{

	}

	commandPool::commandPool(commandPool&& other) noexcept {
		m_commandPool = other.m_commandPool;
		m_queueFamilyIndex = other.m_queueFamilyIndex;
		m_device = other.m_device;

		other.m_commandPool = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	commandPool& commandPool::operator=(commandPool&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_commandPool)
			vkDestroyCommandPool(m_device, m_commandPool, nullptr);

		m_commandPool = other.m_commandPool;
		m_queueFamilyIndex = other.m_queueFamilyIndex;
		m_device = other.m_device;

		other.m_commandPool = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;

		return *this;
	}

	commandPool::~commandPool() noexcept {
		if (m_commandPool)
			vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	}

	VkCommandPool commandPool::native_commandPool() const noexcept {
		return m_commandPool;
	}

}