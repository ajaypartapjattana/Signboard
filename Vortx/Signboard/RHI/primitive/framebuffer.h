#pragma once

namespace rhi::procedure {
	class framebuffer_creator;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {
	
	struct framebuffer_vkAccess;

	class framebuffer {
	public:
		framebuffer() noexcept;

		framebuffer(const framebuffer&) = delete;
		framebuffer& operator=(const framebuffer&) = delete;

		framebuffer(framebuffer&&) noexcept;
		framebuffer& operator=(framebuffer&&) noexcept;

		~framebuffer() noexcept;

	private:
		friend class rhi::procedure::framebuffer_creator;
		friend struct framebuffer_vkAccess;

		VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

		VkDevice m_device = VK_NULL_HANDLE;

	};

}