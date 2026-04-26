#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Core/Containers/storage.h"

class vertexFields {
public:
	vertexFields() noexcept;

	vertexFields(const vertexFields&) = delete;
	vertexFields& operator=(const vertexFields&) = delete;

	struct createInfo {
		std::vector<VkFormat> orderedFormats;
	};

	uint32_t createVertexLayout(const createInfo& info);

	ctnr::vltView<rhi::pmvVertexLayout> read_vertexLayouts() const noexcept;

private:
	ctnr::vault<rhi::pmvVertexLayout> m_vertexLayouts;
	ctnr::vltWrite<rhi::pmvVertexLayout> m_writer;

};