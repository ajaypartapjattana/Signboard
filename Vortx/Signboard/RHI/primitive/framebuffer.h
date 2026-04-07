#pragma once

#include <vulkan/vulkan.h>
	
namespace rhi::access {
	struct framebuffer_pAccess;
}

namespace rhi {

	class pcdFramebufferCreator;

	class pmvFramebuffer {
	public:
		pmvFramebuffer() noexcept;

		pmvFramebuffer(const pmvFramebuffer&) = delete;
		pmvFramebuffer& operator=(const pmvFramebuffer&) = delete;

		pmvFramebuffer(pmvFramebuffer&&) noexcept;
		pmvFramebuffer& operator=(pmvFramebuffer&&) noexcept;

		~pmvFramebuffer() noexcept;

	private:
		friend class pcdFramebufferCreator;
		friend struct rhi::access::framebuffer_pAccess;

		VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
		VkExtent2D m_extent;

		VkDevice _dvc = VK_NULL_HANDLE;

	};

}