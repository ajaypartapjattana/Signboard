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
		other.r_device = VK_NULL_HANDLE;
	}

	pmvPipelineLayout& pmvPipelineLayout::operator=(pmvPipelineLayout&& other) noexcept {
		if (this == &other)
			return *this;

		if (r_device)
			vkDestroyPipelineLayout(r_device, m_layout, nullptr);

		m_layout = other.m_layout;
		r_device = other.r_device;

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	pmvPipelineLayout::~pmvPipelineLayout() noexcept {
		if (r_device)
			vkDestroyPipelineLayout(r_device, m_layout, nullptr);
	}

	void pmvPipelineLayout::reset() noexcept {
		if (r_device) {
			vkDestroyPipelineLayout(r_device, m_layout, nullptr);
			r_device = VK_NULL_HANDLE;
		}

		m_layout = VK_NULL_HANDLE;
	}

}