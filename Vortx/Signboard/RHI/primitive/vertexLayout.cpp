#include "vertexLayout.h"

#include <stdexcept>

namespace rhi {

	pmvVertexLayout::pmvVertexLayout() noexcept
		:
		m_stride(0)
	{

	}

	void pmvVertexLayout::addAttribute(VkFormat format) {
		VertexAttribute attribute{};
		attribute.location = static_cast<uint32_t>(m_attributes.size());
		attribute.format = format;
		attribute.offset = m_stride;

		m_attributes.push_back(attribute);

		m_stride += formatSize(format);
	}

	uint32_t pmvVertexLayout::stride() const {

	}

	uint32_t pmvVertexLayout::formatSize(VkFormat format) const {
		switch (format) {
		case VK_FORMAT_R32_SFLOAT: return 4;
		case VK_FORMAT_R32G32_SFLOAT: return 8;
		case VK_FORMAT_R32G32B32_SFLOAT: return 12;
		case VK_FORMAT_R32G32B32A32_SFLOAT: return 16;

		case VK_FORMAT_R8_UINT: return 1;
		case VK_FORMAT_R8G8_UINT: return 2;
		case VK_FORMAT_R8G8B8_UINT: return 3;
		case VK_FORMAT_R8G8B8A8_UINT: return 4;

		default:
			throw std::runtime_error("LOGIC: unsupported_format!");
		}
	}

}