#include "image.h"

namespace rhi {
	
	pmvImage::pmvImage() noexcept 
		:
		m_image(VK_NULL_HANDLE),
		m_view(VK_NULL_HANDLE),
		_alloc(VK_NULL_HANDLE),

		m_extent(),
		m_format(VK_FORMAT_UNDEFINED),

		_dvc(VK_NULL_HANDLE),
		_allctr(VK_NULL_HANDLE)
	{

	}

	pmvImage::pmvImage(pmvImage&& other) noexcept 
		:
		m_image(other.m_image),
		m_view(other.m_view),
		_alloc(other._alloc),

		m_extent(other.m_extent),
		m_format(other.m_format),

		_dvc(other._dvc),
		_allctr(other._allctr)
	{
		other.m_image = VK_NULL_HANDLE;
		other.m_view = VK_NULL_HANDLE;
	}

	pmvImage& pmvImage::operator=(pmvImage&& other) noexcept {
		if (this == &other)
			return *this;

		if (m_view)
			vkDestroyImageView(_dvc, m_view, nullptr);

		if (m_image)
			vmaDestroyImage(_allctr, m_image, _alloc);

		m_image = other.m_image;
		m_view = other.m_view;
		_alloc = other._alloc;
		
		_dvc = other._dvc;
		_allctr = other._allctr;

		other.m_image = VK_NULL_HANDLE;
		other.m_view = VK_NULL_HANDLE;

		return *this;
	}

	pmvImage::~pmvImage() noexcept {
		if (m_view)
			vkDestroyImageView(_dvc, m_view, nullptr);
		
		if (m_image && _alloc)
			vmaDestroyImage(_allctr, m_image, _alloc);
	}

}