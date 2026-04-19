#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct renderPass_pAccess;
}

namespace rhi {

	class pcdRenderPassCreate;

	class pmvRenderPass {
	public:
		pmvRenderPass() noexcept;

		pmvRenderPass(const pmvRenderPass&) = delete;
		pmvRenderPass& operator=(const pmvRenderPass&) = delete;

		pmvRenderPass(pmvRenderPass&&) noexcept;
		pmvRenderPass& operator=(pmvRenderPass&&) noexcept;

		~pmvRenderPass() noexcept;

	private:
		friend class pcdRenderPassCreate;
		friend struct rhi::access::renderPass_pAccess;

		VkRenderPass m_renderPass;
		std::vector<VkFormat> ordered_attachmentFormats;

		VkDevice r_device;

	};

}