#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class VertexLayout {
	public:
		void addAttribute();
		uint32_t stride() const;

	private:
		struct VertexAttribute {
			uint32_t location;
			VkFormat format;
			uint32_t offset;
		};

		std::vector<VertexAttribute> attrubute;
		uint32_t stride;

	};

}
