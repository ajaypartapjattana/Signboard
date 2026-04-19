#include "renderPass.h"

namespace rhi {

	pmvRenderPass::pmvRenderPass() noexcept
		: 
		m_renderPass(VK_NULL_HANDLE), 
		r_device(VK_NULL_HANDLE)
	{

	}

	pmvRenderPass::pmvRenderPass(pmvRenderPass&& other) noexcept 
		:
		m_renderPass(other.m_renderPass),
		r_device(other.r_device)
	{
		other.m_renderPass = VK_NULL_HANDLE;
	}

	pmvRenderPass& pmvRenderPass::operator=(pmvRenderPass&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_renderPass)
			vkDestroyRenderPass(r_device, m_renderPass, nullptr);

		m_renderPass = other.m_renderPass;
		r_device = other.r_device;

		other.m_renderPass = VK_NULL_HANDLE;

		return *this;
	}

	pmvRenderPass::~pmvRenderPass() noexcept {
		if (m_renderPass)
			vkDestroyRenderPass(r_device, m_renderPass, nullptr);
	}

}