#include "image.h"

namespace rhi {
	
	pmvImage::pmvImage() noexcept 
		:
		m_image(VK_NULL_HANDLE),
		m_view(VK_NULL_HANDLE),
		m_allocation(VK_NULL_HANDLE),

		extent(),
		format(VK_FORMAT_UNDEFINED),

		r_device(VK_NULL_HANDLE),
		r_allocator(VK_NULL_HANDLE)
	{

	}

	pmvImage::pmvImage(pmvImage&& other) noexcept 
		:
		m_image(other.m_image),
		m_view(other.m_view),
		m_allocation(other.m_allocation),

		extent(other.extent),
		format(other.format),

		r_device(other.r_device),
		r_allocator(other.r_allocator)
	{
		other.m_image = VK_NULL_HANDLE;
		other.m_view = VK_NULL_HANDLE;
	}

	pmvImage& pmvImage::operator=(pmvImage&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_view)
			vkDestroyImageView(r_device, m_view, nullptr);

		if (r_allocator && m_allocation)
			vmaDestroyImage(r_allocator, m_image, m_allocation);

		m_image = other.m_image;
		m_view = other.m_view;
		m_allocation = other.m_allocation;
		
		r_device = other.r_device;
		r_allocator = other.r_allocator;

		other.m_image = VK_NULL_HANDLE;
		other.m_view = VK_NULL_HANDLE;

		return *this;
	}

	pmvImage::~pmvImage() noexcept {
		if (m_view)
			vkDestroyImageView(r_device, m_view, nullptr);
		
		if (r_allocator && m_allocation)
			vmaDestroyImage(r_allocator, m_image, m_allocation);
	}

	void pmvImage::reset() noexcept {
		if (m_view) {
			vkDestroyImageView(r_device, m_view, nullptr);
			m_view = VK_NULL_HANDLE;
		}

		if (r_allocator && m_allocation) {
			vmaDestroyImage(r_allocator, m_image, m_allocation);
			m_allocation = VK_NULL_HANDLE;
			m_image = VK_NULL_HANDLE;
		}
	}


}