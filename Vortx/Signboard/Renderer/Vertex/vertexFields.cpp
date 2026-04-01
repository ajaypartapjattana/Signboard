#include "vertexFields.h"

vertexFields::vertexFields() noexcept 
	:
	m_writer(m_vertexLayouts)
{

}

uint32_t vertexFields::createVertexLayout(const createInfo& info) {
	rhi::pmvVertexLayout l_layout;
	
	uint32_t attributeCount = static_cast<uint32_t>(info.orderedFormats.size());
	for (uint32_t i = 0; i < attributeCount; ++i) {
		l_layout.addAttribute(i, info.orderedFormats[i]);
	}

	return m_writer.create(l_layout);
}

ctnr::vltView<rhi::pmvVertexLayout> vertexFields::read_vertexLayouts() const noexcept {
	return ctnr::vltView<rhi::pmvVertexLayout>{ m_vertexLayouts };
}
