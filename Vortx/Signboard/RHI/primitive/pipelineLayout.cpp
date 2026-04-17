#include "pipelineLayout.h"

namespace rhi {

	pmvPipelineLayout::pmvPipelineLayout() noexcept
		: 
		m_layout(VK_NULL_HANDLE), 
		r_device(VK_NULL_HANDLE)
	{
		
	}

	pmvPipelineLayout::pmvPipelineLayout(pmvPipelineLayout&& other) noexcept 
		:
		m_layout(other.m_layout),
		r_device(other.r_device)
	{
		other.m_layout = VK_NULL_HANDLE;
	}

	pmvPipelineLayout& pmvPipelineLayout::operator=(pmvPipelineLayout&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_layout)
			vkDestroyPipelineLayout(r_device, m_layout, nullptr);

		m_layout = other.m_layout;
		r_device = other.r_device;

		other.m_layout = VK_NULL_HANDLE;

		return *this;
	}

	pmvPipelineLayout::~pmvPipelineLayout() noexcept {
		if (m_layout)
			vkDestroyPipelineLayout(r_device, m_layout, nullptr);
	}

}