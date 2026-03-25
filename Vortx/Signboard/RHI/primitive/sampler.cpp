#include "sampler.h"

namespace rhi {

	pmvSampler::pmvSampler() noexcept 
		:
		m_sampler(VK_NULL_HANDLE),
		m_device(VK_NULL_HANDLE)
	{

	}

	pmvSampler::pmvSampler(pmvSampler&& other) noexcept 
		:m_sampler(other.m_sampler),
		m_device(other.m_device)
	{
		other.m_sampler = VK_NULL_HANDLE;
	}

	pmvSampler& pmvSampler::operator=(pmvSampler&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_sampler)
			vkDestroySampler(m_device, m_sampler, nullptr);

		m_sampler = other.m_sampler;
		m_device = other.m_device;

		other.m_sampler = VK_NULL_HANDLE;

		return *this;
	}

	pmvSampler::~pmvSampler() noexcept {
		if (m_sampler)
			vkDestroySampler(m_device, m_sampler, nullptr);
	}

}