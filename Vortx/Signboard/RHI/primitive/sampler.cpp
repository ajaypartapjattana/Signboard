#include "sampler.h"

namespace rhi::primitive {

	sampler::sampler(sampler&& other) noexcept {
		m_sampler = other.m_sampler;
		m_device = other.m_device;

		other.m_sampler = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	sampler& sampler::operator=(sampler&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_sampler)
			vkDestroySampler(m_device, m_sampler, nullptr);

		m_sampler = other.m_sampler;
		m_device = other.m_device;

		other.m_sampler = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;

		return *this;
	}

	sampler::~sampler() noexcept {
		if (m_sampler)
			vkDestroySampler(m_device, m_sampler, nullptr);
	}

	VkSampler sampler::native_sampler() const noexcept {
		return m_sampler;
	}

}