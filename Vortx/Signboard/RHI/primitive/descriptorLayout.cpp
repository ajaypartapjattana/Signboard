#include "descriptorLayout.h"

namespace rhi::primitive {

	descriptorLayout::descriptorLayout() noexcept
		: m_setLayout(VK_NULL_HANDLE), m_device(VK_NULL_HANDLE)
	{

	}

	descriptorLayout::descriptorLayout(descriptorLayout&& other) noexcept {
		m_setLayout = other.m_setLayout;
		m_device = other.m_device;

		other.m_setLayout = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	descriptorLayout& descriptorLayout::operator=(descriptorLayout&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_setLayout)
			vkDestroyDescriptorSetLayout(m_device, m_setLayout, nullptr);

		m_setLayout = other.m_setLayout;
		m_device = other.m_device;

		other.m_setLayout = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;

		return *this;
	}

	descriptorLayout::~descriptorLayout() noexcept {
		if (m_setLayout)
			vkDestroyDescriptorSetLayout(m_device, m_setLayout, nullptr);
	}

	VkDescriptorSetLayout descriptorLayout::native_setLayout() const noexcept {
		return m_setLayout;
	}

}