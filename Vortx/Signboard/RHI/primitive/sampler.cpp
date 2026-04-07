#include "sampler.h"

namespace rhi {

	pmvSampler::pmvSampler() noexcept 
		:
		m_sampler(VK_NULL_HANDLE),
		_dvc(VK_NULL_HANDLE)
	{

	}

	pmvSampler::pmvSampler(pmvSampler&& other) noexcept 
		:m_sampler(other.m_sampler),
		_dvc(other._dvc)
	{
		other.m_sampler = VK_NULL_HANDLE;
	}

	pmvSampler& pmvSampler::operator=(pmvSampler&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_sampler)
			vkDestroySampler(_dvc, m_sampler, nullptr);

		m_sampler = other.m_sampler;
		_dvc = other._dvc;

		other.m_sampler = VK_NULL_HANDLE;

		return *this;
	}

	pmvSampler::~pmvSampler() noexcept {
		if (m_sampler)
			vkDestroySampler(_dvc, m_sampler, nullptr);
	}

}