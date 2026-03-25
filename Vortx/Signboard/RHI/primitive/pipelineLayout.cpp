#include "pipelineLayout.h"

namespace rhi {

	pmvPipelineLayout::pmvPipelineLayout() noexcept
		: 
		m_layout(VK_NULL_HANDLE), 
		m_device(VK_NULL_HANDLE)
	{
		
	}

	pmvPipelineLayout::pmvPipelineLayout(pmvPipelineLayout&& other) noexcept 
		:
		m_layout(other.m_layout),
		m_device(other.m_device)
	{
		other.m_layout = VK_NULL_HANDLE;
	}

	pmvPipelineLayout& pmvPipelineLayout::operator=(pmvPipelineLayout&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_layout)
			vkDestroyPipelineLayout(m_device, m_layout, nullptr);

		m_layout = other.m_layout;
		m_device = other.m_device;

		other.m_layout = VK_NULL_HANDLE;

		return *this;
	}

	pmvPipelineLayout::~pmvPipelineLayout() noexcept {
		if (m_layout)
			vkDestroyPipelineLayout(m_device, m_layout, nullptr);
	}

}