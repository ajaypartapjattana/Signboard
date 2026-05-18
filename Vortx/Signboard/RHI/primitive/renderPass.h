#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	struct _pAccess;

	class pcdRenderPassCreate;

	class pmvRenderPass {
	public:
		pmvRenderPass() noexcept;

		pmvRenderPass(const pmvRenderPass&) = delete;
		pmvRenderPass& operator=(const pmvRenderPass&) = delete;

		pmvRenderPass(pmvRenderPass&&) noexcept;
		pmvRenderPass& operator=(pmvRenderPass&&) noexcept;

		~pmvRenderPass() noexcept;

		void reset() noexcept;

	private:
		friend class pcdRenderPassCreate;
		friend struct _pAccess;

		VkRenderPass m_renderPass;
		uint32_t attachmentCount;

		VkDevice r_device;

	};

}