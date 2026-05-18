#include "sampler.h"

namespace rhi {

	pmvSampler::pmvSampler() noexcept 
		:
		m_sampler(VK_NULL_HANDLE),
		r_device(VK_NULL_HANDLE)
	{

	}

	pmvSampler::pmvSampler(const pmvSampler& other) noexcept 
		:
		m_sampler(other.m_sampler),
		r_device(VK_NULL_HANDLE)
	{

	}

	pmvSampler& pmvSampler::operator=(const pmvSampler& other) noexcept {
		if (this == &other)
			return *this;

		if (r_device)
			vkDestroySampler(r_device, m_sampler, nullptr);

		m_sampler = other.m_sampler;
		r_device = VK_NULL_HANDLE;

		return *this;
	}

	pmvSampler::pmvSampler(pmvSampler&& other) noexcept 
		:
		m_sampler(other.m_sampler),
		r_device(other.r_device)
	{
		other.r_device = VK_NULL_HANDLE;
	}

	pmvSampler& pmvSampler::operator=(pmvSampler&& other) noexcept {
		if (this == &other)
			return *this;

		if (r_device)
			vkDestroySampler(r_device, m_sampler, nullptr);

		m_sampler = other.m_sampler;
		r_device = other.r_device;

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	pmvSampler::~pmvSampler() noexcept {
		if (r_device)
			vkDestroySampler(r_device, m_sampler, nullptr);
	}

	void pmvSampler::reset() noexcept {
		if (r_device) {
			vkDestroySampler(r_device, m_sampler, nullptr);
			r_device = VK_NULL_HANDLE;
		}

		m_sampler = VK_NULL_HANDLE;
	}


}