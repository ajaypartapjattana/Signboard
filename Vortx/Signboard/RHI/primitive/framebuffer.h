#pragma once

#include <vulkan/vulkan.h>
	
namespace rhi {

	struct _pAccess;

	class pcdFramebufferCreate;

	class pmvFramebuffer {
	public:
		pmvFramebuffer() noexcept;

		pmvFramebuffer(const pmvFramebuffer&) = delete;
		pmvFramebuffer& operator=(const pmvFramebuffer&) = delete;

		pmvFramebuffer(pmvFramebuffer&&) noexcept;
		pmvFramebuffer& operator=(pmvFramebuffer&&) noexcept;

		~pmvFramebuffer() noexcept;

		void reset() noexcept;

	private:
		friend class pcdFramebufferCreate;
		friend struct _pAccess;

		VkFramebuffer m_framebuffer;
		VkExtent2D extent;

		VkDevice r_device;

	};

}