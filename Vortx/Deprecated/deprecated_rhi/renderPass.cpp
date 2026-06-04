#include "renderPass.h"

namespace rhi {

	pmvRenderPass::pmvRenderPass() noexcept
		: 
		m_renderPass(VK_NULL_HANDLE), 
		attachmentCount(0),
		r_device(VK_NULL_HANDLE)
	{

	}

	pmvRenderPass::pmvRenderPass(pmvRenderPass&& other) noexcept 
		:
		m_renderPass(other.m_renderPass),
		attachmentCount(other.attachmentCount),
		r_device(other.r_device)
	{
		other.r_device = VK_NULL_HANDLE;
	}

	pmvRenderPass& pmvRenderPass::operator=(pmvRenderPass&& other) noexcept {
		if (this == &other)
			return *this;

		if (r_device)
			vkDestroyRenderPass(r_device, m_renderPass, nullptr);

		m_renderPass = other.m_renderPass;
		attachmentCount = other.attachmentCount;
		r_device = other.r_device;

		other.r_device = VK_NULL_HANDLE;

		return *this;
	}

	pmvRenderPass::~pmvRenderPass() noexcept {
		if (r_device)
			vkDestroyRenderPass(r_device, m_renderPass, nullptr);
	}

	void pmvRenderPass::reset() noexcept {
		if (r_device) {
			vkDestroyRenderPass(r_device, m_renderPass, nullptr);
		}

		r_device = VK_NULL_HANDLE;
	}

}