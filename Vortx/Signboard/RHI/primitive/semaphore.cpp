#include "semaphore.h"

#include "Signboard/RHI/core/device_vk.h"

namespace rhi::primitive {

	semaphore::semaphore(const rhi::core::device& device) noexcept
		:
		m_semaphore(VK_NULL_HANDLE),
		m_device(rhi::core::device_vkAccess::get(device))
	{
		VkSemaphoreCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		vkCreateSemaphore(m_device, &createInfo, nullptr, &m_semaphore);
	}

	semaphore::semaphore(semaphore&& other) noexcept 
		: 
		m_semaphore(other.m_semaphore),
		m_device(other.m_device)
	{
		other.m_semaphore = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	semaphore& semaphore::operator=(semaphore&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_semaphore)
			vkDestroySemaphore(m_device, m_semaphore, nullptr);

		m_semaphore = other.m_semaphore;
		m_device = other.m_device;

		other.m_semaphore = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;

		return *this;
	}

	semaphore::~semaphore() noexcept {
		if (m_semaphore)
			vkDestroySemaphore(m_device, m_semaphore, nullptr);
	}

}