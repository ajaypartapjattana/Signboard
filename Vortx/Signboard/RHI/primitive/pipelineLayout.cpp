#include "pipelineLayout.h"

namespace rhi::primitive {

	pipelineLayout::pipelineLayout() noexcept
		: m_layout(VK_NULL_HANDLE), m_device(VK_NULL_HANDLE)
	{
		
	}

	pipelineLayout::pipelineLayout(pipelineLayout&& other) noexcept {
		m_layout = other.m_layout;
		m_device = other.m_device;

		other.m_layout = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	pipelineLayout& pipelineLayout::operator=(pipelineLayout&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_layout)
			vkDestroyPipelineLayout(m_device, m_layout, nullptr);

		m_layout = other.m_layout;
		m_device = other.m_device;

		other.m_layout = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;

		return *this;
	}

	pipelineLayout::~pipelineLayout() noexcept {
		if (m_layout)
			vkDestroyPipelineLayout(m_device, m_layout, nullptr);
	}

	VkPipelineLayout pipelineLayout::native_layout() const noexcept {
		return m_layout;
	}

}