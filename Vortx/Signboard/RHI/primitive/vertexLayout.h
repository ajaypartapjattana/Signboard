#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	namespace access {
		struct vertexLayout_pAccess;
	}

	class pmvVertexLayout {
	public:
		pmvVertexLayout() noexcept;

		void addAttribute(VkFormat format);
		uint32_t stride() const;

	private:
		uint32_t formatSize(VkFormat format) const;

	private:
		friend struct rhi::access::vertexLayout_pAccess;

		struct VertexAttribute {
			uint32_t location;
			VkFormat format;
			uint32_t offset;
		};

		std::vector<VertexAttribute> m_attributes;
		uint32_t m_stride;

	};

}
