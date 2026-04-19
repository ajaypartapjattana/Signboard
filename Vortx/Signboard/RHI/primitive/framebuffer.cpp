#include "framebuffer.h"

namespace rhi {

	pmvFramebuffer::pmvFramebuffer() noexcept
		: 
		m_framebuffer(VK_NULL_HANDLE),
		extent(),
		r_device(VK_NULL_HANDLE)
	{

	}

	pmvFramebuffer::pmvFramebuffer(pmvFramebuffer&& other) noexcept
		: 
		m_framebuffer(other.m_framebuffer),
		extent(other.extent),
		r_device(other.r_device)
	{
		other.m_framebuffer = VK_NULL_HANDLE;
	}

	pmvFramebuffer& pmvFramebuffer::operator=(pmvFramebuffer&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_framebuffer)
			vkDestroyFramebuffer(r_device, m_framebuffer, nullptr);

		m_framebuffer = other.m_framebuffer;
		extent = other.extent;
		r_device = other.r_device;

		other.m_framebuffer = VK_NULL_HANDLE;

		return *this;
	}

	pmvFramebuffer::~pmvFramebuffer() noexcept {
		if (m_framebuffer)
			vkDestroyFramebuffer(r_device, m_framebuffer, nullptr);
	}

}