#include "framebuffer.h"

namespace rhi::primitive {

	framebuffer::framebuffer() noexcept
		: m_framebuffer(VK_NULL_HANDLE), m_device(VK_NULL_HANDLE)
	{

	}

	framebuffer::framebuffer(framebuffer&& other) noexcept
		: m_framebuffer(other.m_framebuffer), m_device(other.m_device)
	{
		other.m_framebuffer = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;
	}

	framebuffer& framebuffer::operator=(framebuffer&& other) noexcept {
		if (this == &other)
			return *this;

		m_framebuffer = other.m_framebuffer;
		m_device = other.m_device;

		other.m_framebuffer = VK_NULL_HANDLE;
		other.m_device = VK_NULL_HANDLE;

		return *this;
	}

	framebuffer::~framebuffer() noexcept {
		if (m_framebuffer)
			vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
	}

}