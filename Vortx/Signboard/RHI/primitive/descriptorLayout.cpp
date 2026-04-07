#include "descriptorLayout.h"

namespace rhi {

	pmvDescriptorLayout::pmvDescriptorLayout() noexcept
		: 
		m_setLayout(VK_NULL_HANDLE), 
		_dvc(VK_NULL_HANDLE)
	{

	}

	pmvDescriptorLayout::pmvDescriptorLayout(pmvDescriptorLayout&& other) noexcept 
		:
		m_setLayout(other.m_setLayout),
		_dvc(other._dvc)
	{
		other.m_setLayout = VK_NULL_HANDLE;
	}

	pmvDescriptorLayout& pmvDescriptorLayout::operator=(pmvDescriptorLayout&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_setLayout)
			vkDestroyDescriptorSetLayout(_dvc, m_setLayout, nullptr);

		m_setLayout = other.m_setLayout;
		_dvc = other._dvc;

		other.m_setLayout = VK_NULL_HANDLE;

		return *this;
	}

	pmvDescriptorLayout::~pmvDescriptorLayout() noexcept {
		if (m_setLayout)
			vkDestroyDescriptorSetLayout(_dvc, m_setLayout, nullptr);
	}

}