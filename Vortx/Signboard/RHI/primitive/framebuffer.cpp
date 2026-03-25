#include "framebuffer.h"

namespace rhi {

	pmvFramebuffer::pmvFramebuffer() noexcept
		: 
		m_framebuffer(VK_NULL_HANDLE),
		m_extent(),
		m_device(VK_NULL_HANDLE)
	{

	}

	pmvFramebuffer::pmvFramebuffer(pmvFramebuffer&& other) noexcept
		: 
		m_framebuffer(other.m_framebuffer),
		m_extent(other.m_extent),
		m_device(other.m_device)
	{
		other.m_framebuffer = VK_NULL_HANDLE;
	}

	pmvFramebuffer& pmvFramebuffer::operator=(pmvFramebuffer&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_framebuffer)
			vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);

		m_framebuffer = other.m_framebuffer;
		m_extent = other.m_extent;
		m_device = other.m_device;

		other.m_framebuffer = VK_NULL_HANDLE;

		return *this;
	}

	pmvFramebuffer::~pmvFramebuffer() noexcept {
		if (m_framebuffer)
			vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
	}

}