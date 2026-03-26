#pragma once

#include "vertexLayout.h"

struct VertexInputDescription {
	VkVertexInputBindingDescription binding;
	std::vector<VkVertexInputAttributeDescription> attributes;
};

namespace rhi::access {

	struct vertexLayout_pAccess {

		static VertexInputDescription getDescription(const pmvVertexLayout& vl) {
			VertexInputDescription desc{};

			desc.binding.binding = 0;
			desc.binding.stride = vl.m_stride;
			desc.binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			uint32_t attributeCount = static_cast<uint32_t>(vl.m_attributes.size());
			desc.attributes.resize(attributeCount);

			for (uint32_t i = 0; i < attributeCount; ++i) {
				const auto& attribute = vl.m_attributes[i];
				auto& att = desc.attributes[i];

				att.binding = 0;
				att.format = attribute.format;
				att.location = attribute.location;
				att.offset = attribute.offset;
			}

			return desc;
		}

	};

}