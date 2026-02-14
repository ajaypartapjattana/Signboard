#include "image.h"

namespace rhi::primitive {
	
	image::image() noexcept {

	}

	image::image(image&& other) noexcept {
		m_image = other.m_image;
		m_view = other.m_view;
		m_allocation = other.m_allocation;

		m_device = other.m_device;
		m_allocator = other.m_allocator;

		other.m_image = VK_NULL_HANDLE;
		other.m_view = VK_NULL_HANDLE;
		other.m_allocation = VK_NULL_HANDLE;
		
		other.m_device = VK_NULL_HANDLE;
		other.m_allocator = VK_NULL_HANDLE;
	}

	image& image::operator=(image&& other) noexcept {
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
		other.m_allocation = VK_NULL_HANDLE;
		
		other.m_device = VK_NULL_HANDLE;
		other.m_allocator = VK_NULL_HANDLE;

		return *this;
	}

	image::~image() noexcept {
		if (m_view)
			vkDestroyImageView(m_device, m_view, nullptr);
		
		if (m_image && m_allocation)
			vmaDestroyImage(m_allocator, m_image, m_allocation);
	}

	const VkImage image::native_image() const noexcept {
		return m_image;
	}

	const VkImageView image::native_view() const noexcept {
		return m_view;
	}

}