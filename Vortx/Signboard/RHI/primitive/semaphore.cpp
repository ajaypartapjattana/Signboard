#include "semaphore.h"

#include "Signboard/RHI/core/device_pAccess.h"

namespace rhi {

	pmvSemaphore::pmvSemaphore() noexcept 
		:
		m_semaphore(VK_NULL_HANDLE),
		_dvc(VK_NULL_HANDLE)
	{

	}

	pmvSemaphore::pmvSemaphore(const rhi::creDevice& device) noexcept
		:
		m_semaphore(VK_NULL_HANDLE),
		_dvc(rhi::access::device_pAccess::extract(device))
	{
		VkSemaphoreCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		vkCreateSemaphore(_dvc, &createInfo, nullptr, &m_semaphore);
	}

	pmvSemaphore::pmvSemaphore(pmvSemaphore&& other) noexcept 
		: 
		m_semaphore(other.m_semaphore),
		_dvc(other._dvc)
	{
		other.m_semaphore = VK_NULL_HANDLE;
	}

	pmvSemaphore& pmvSemaphore::operator=(pmvSemaphore&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_semaphore)
			vkDestroySemaphore(_dvc, m_semaphore, nullptr);

		m_semaphore = other.m_semaphore;
		_dvc = other._dvc;

		other.m_semaphore = VK_NULL_HANDLE;

		return *this;
	}

	pmvSemaphore::~pmvSemaphore() noexcept {
		if (m_semaphore)
			vkDestroySemaphore(_dvc, m_semaphore, nullptr);
	}

}