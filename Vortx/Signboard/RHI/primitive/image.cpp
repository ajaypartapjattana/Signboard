#include "image.h"

namespace rhi {
	
	pmvImage::pmvImage() noexcept 
		:
		m_image(VK_NULL_HANDLE),
		m_view(VK_NULL_HANDLE),
		m_allocation(VK_NULL_HANDLE),

		m_extent(),
		m_format(VK_FORMAT_UNDEFINED),

		m_device(VK_NULL_HANDLE),
		m_allocator(VK_NULL_HANDLE)
	{

	}

	pmvImage::pmvImage(pmvImage&& other) noexcept 
		:
		m_image(other.m_image),
		m_view(other.m_view),
		m_allocation(other.m_allocation),

		m_extent(other.m_extent),
		m_format(other.m_format),

		m_device(other.m_device),
		m_allocator(other.m_allocator)
	{
		other.m_image = VK_NULL_HANDLE;
		other.m_view = VK_NULL_HANDLE;
	}

	pmvImage& pmvImage::operator=(pmvImage&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_view)
			vkDestroyImageView(m_device, m_view, nullptr);

		if (m_image)
			vmaDestroyImage(m_allocator, m_image, m_allocation);

		m_image = other.m_image;
		m_view = other.m_view;
		m_allocation = other.m_allocation;
		
		m_device = other.m_device;
		m_allocator = other.m_allocator;

		other.m_image = VK_NULL_HANDLE;
		other.m_view = VK_NULL_HANDLE;

		return *this;
	}

	pmvImage::~pmvImage() noexcept {
		if (m_view)
			vkDestroyImageView(m_device, m_view, nullptr);
		
		if (m_image && m_allocation)
			vmaDestroyImage(m_allocator, m_image, m_allocation);
	}

}