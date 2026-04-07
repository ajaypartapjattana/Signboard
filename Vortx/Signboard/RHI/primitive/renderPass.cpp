#include "renderPass.h"

namespace rhi {

	pmvRenderPass::pmvRenderPass() noexcept
		: 
		m_renderPass(VK_NULL_HANDLE), 
		_dvc(VK_NULL_HANDLE)
	{

	}

	pmvRenderPass::pmvRenderPass(pmvRenderPass&& other) noexcept 
		:
		m_renderPass(other.m_renderPass),
		_dvc(other._dvc)
	{
		other.m_renderPass = VK_NULL_HANDLE;
	}

	pmvRenderPass& pmvRenderPass::operator=(pmvRenderPass&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_renderPass)
			vkDestroyRenderPass(_dvc, m_renderPass, nullptr);

		m_renderPass = other.m_renderPass;
		_dvc = other._dvc;

		other.m_renderPass = VK_NULL_HANDLE;

		return *this;
	}

	pmvRenderPass::~pmvRenderPass() noexcept {
		if (m_renderPass)
			vkDestroyRenderPass(_dvc, m_renderPass, nullptr);
	}

}