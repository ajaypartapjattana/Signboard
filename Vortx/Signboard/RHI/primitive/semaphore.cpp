#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pmvSemaphore::pmvSemaphore() noexcept 
		:
		m_semaphore(VK_NULL_HANDLE),
		r_device(VK_NULL_HANDLE)
	{

	}

	pmvSemaphore::pmvSemaphore(const creDevice& device) noexcept
		:
		m_semaphore(VK_NULL_HANDLE),
		r_device(_pAccess::extract(device))
	{
		VkSemaphoreCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		vkCreateSemaphore(r_device, &createInfo, nullptr, &m_semaphore);
	}

	pmvSemaphore::pmvSemaphore(pmvSemaphore&& other) noexcept 
		: 
		m_semaphore(other.m_semaphore),
		r_device(other.r_device)
	{
		other.m_semaphore = VK_NULL_HANDLE;
	}

	pmvSemaphore& pmvSemaphore::operator=(pmvSemaphore&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_semaphore)
			vkDestroySemaphore(r_device, m_semaphore, nullptr);

		m_semaphore = other.m_semaphore;
		r_device = other.r_device;

		other.m_semaphore = VK_NULL_HANDLE;

		return *this;
	}

	pmvSemaphore::~pmvSemaphore() noexcept {
		if (m_semaphore)
			vkDestroySemaphore(r_device, m_semaphore, nullptr);
	}

}