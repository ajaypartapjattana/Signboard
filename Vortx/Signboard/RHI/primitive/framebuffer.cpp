#include "framebuffer.h"

namespace rhi {

	pmvFramebuffer::pmvFramebuffer() noexcept
		: 
		m_framebuffer(VK_NULL_HANDLE),
		m_extent(),
		_dvc(VK_NULL_HANDLE)
	{

	}

	pmvFramebuffer::pmvFramebuffer(pmvFramebuffer&& other) noexcept
		: 
		m_framebuffer(other.m_framebuffer),
		m_extent(other.m_extent),
		_dvc(other._dvc)
	{
		other.m_framebuffer = VK_NULL_HANDLE;
	}

	pmvFramebuffer& pmvFramebuffer::operator=(pmvFramebuffer&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_framebuffer)
			vkDestroyFramebuffer(_dvc, m_framebuffer, nullptr);

		m_framebuffer = other.m_framebuffer;
		m_extent = other.m_extent;
		_dvc = other._dvc;

		other.m_framebuffer = VK_NULL_HANDLE;

		return *this;
	}

	pmvFramebuffer::~pmvFramebuffer() noexcept {
		if (m_framebuffer)
			vkDestroyFramebuffer(_dvc, m_framebuffer, nullptr);
	}

}