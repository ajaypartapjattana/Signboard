#include "Signboard/RHI/Internal/rhi_pAccess.h"

#include <stdexcept>

namespace rhi {

	pmvSemaphorePool::pmvSemaphorePool(const device& device, uint32_t capacity, VkSemaphoreCreateInfo* pCreateInfo) noexcept
		:
		pInfo(allot_basic(pCreateInfo)),
		r_device(_pAccess::extract(device))
	{
		resize(capacity);
	}

	pmvSemaphorePool::pmvSemaphorePool(const device& device, VkSemaphoreCreateInfo* pCreateInfo) noexcept 
		:
		pInfo(allot_basic(pCreateInfo)),
		r_device(_pAccess::extract(device))
	{

	}

	VkSemaphoreCreateInfo* pmvSemaphorePool::allot_basic(VkSemaphoreCreateInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkSemaphoreCreateInfo>();

		VkSemaphoreCreateInfo* info = m_ownedInfo.get();
		info->sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		return info;
	}

	pmvSemaphorePool::pmvSemaphorePool(pmvSemaphorePool&& other) noexcept 
		:
		m_semaphores(std::move(other.m_semaphores)),
		m_ownedInfo(std::move(other.m_ownedInfo)),
		pInfo(other.pInfo),
		r_device(other.r_device)
	{

	}

	pmvSemaphorePool& pmvSemaphorePool::operator=(pmvSemaphorePool&& other) noexcept {
		if (this == &other)
			return *this;

		size_t _sSz = m_semaphores.size();

		for (size_t i = 0; i < _sSz; ++i) {
			vkDestroySemaphore(r_device, m_semaphores[i], nullptr);
		}

		m_semaphores = std::move(other.m_semaphores);
		m_ownedInfo = std::move(other.m_ownedInfo);
		pInfo = other.pInfo;
		r_device = other.r_device;

		return *this;
	}

	pmvSemaphorePool::~pmvSemaphorePool() noexcept {
		size_t _sSz = m_semaphores.size();

		for (size_t i = 0; i < _sSz; ++i) {
			vkDestroySemaphore(r_device, m_semaphores[i], nullptr);
		}
	}

	void pmvSemaphorePool::root(const device& device) {
		VkDevice _device = _pAccess::extract(device);

		uint32_t _sSz = static_cast<uint32_t>(m_semaphores.size());
		std::vector<VkSemaphore> semaphores(_sSz);

		VkResult result;
		for (uint32_t i = 0; i < _sSz; ++i) {
			result = vkCreateSemaphore(_device, pInfo, nullptr, &semaphores[i]);
			if (result == VK_SUCCESS)
				continue;

			for (uint32_t j = 0; j < i; ++j) {
				vkDestroySemaphore(_device, semaphores[j], nullptr);
			}

			throw std::runtime_error("FAILURE: semaphorePool_re_rooting!");
		}

		for (uint32_t i = 0; i < _sSz; ++i) {
			vkDestroySemaphore(r_device, m_semaphores[i], nullptr);
		}
		
		m_semaphores.swap(semaphores);

		r_device = _device;
	}

	void pmvSemaphorePool::resize(uint32_t size) {
		uint32_t _sSz = static_cast<uint32_t>(m_semaphores.size());
		
		if (size == _sSz)
			return;

		if (size < _sSz) {
			for (uint32_t i = size; i < _sSz; ++i) {
				vkDestroySemaphore(r_device, m_semaphores[i], nullptr);
			}

			m_semaphores.resize(size);

			return;
		}

		m_semaphores.resize(size);

		VkResult result;
		for (uint32_t i = _sSz; i < size; ++i) {
			result = vkCreateSemaphore(r_device, pInfo, nullptr, &m_semaphores[i]);
			if (result == VK_SUCCESS) {
				continue;
			}

			for (uint32_t j = _sSz; j < i; ++j) {
				vkDestroySemaphore(r_device, m_semaphores[j], nullptr);
			}

			m_semaphores.resize(_sSz);

			throw std::runtime_error("FAILURE: semaphorePool_resize!");
		}

		m_size = size;
	}

	VkSemaphore pmvSemaphorePool::grant() noexcept {
		if (next >= m_size)
			resize(m_size ? m_size * 2 : 1);

		return m_semaphores[next++];
	}

	void pmvSemaphorePool::reuse() noexcept {
		next = 0;
	}

}