#include "renderPass.h"

namespace rhi::primitive {

	renderPass::renderPass() noexcept
		: m_renderPass(VK_NULL_HANDLE), m_device(VK_NULL_HANDLE)
	{

	}

	renderPass::renderPass(renderPass&& other) noexcept {
		m_renderPass = other.m_renderPass;
		m_device = other.m_device;

		other.m_renderPass = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	renderPass& renderPass::operator=(renderPass&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_renderPass)
			vkDestroyRenderPass(m_device, m_renderPass, nullptr);

		m_renderPass = other.m_renderPass;
		m_device = other.m_device;

		other.m_renderPass = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;

		return *this;
	}

	renderPass::~renderPass() noexcept {
		if (m_renderPass)
			vkDestroyRenderPass(m_device, m_renderPass, nullptr);
	}

	VkRenderPass renderPass::native_renderPass() const noexcept {
		return m_renderPass;
	}

}