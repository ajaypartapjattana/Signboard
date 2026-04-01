#include "resources.h"

#include "Signboard/Renderer/RenderBackend/rndr_context_Access.h"

Resources::Resources(const rndr_context& context, const rndr_methods& methods) noexcept
	:
	m_buffers(rndr_context_Access::get_allocator(context)),
	m_images(rndr_context_Access::get_device(context), rndr_context_Access::get_allocator(context)),

	m_uploader(m_buffers),

	m_vertWriter(m_vertBuffers)
{
	createDefaultVertBuffer();
}

void Resources::createDefaultVertBuffer() {

}

void Resources::createVertexBuffer(size_t size) {
	rsrc_buffers::createInfo info{};

	info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	info.memory = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	info.size = size;
	
	info.allocationFlags = 0;

	uint32_t bufferIndex = m_buffers.createBuffer(info);

	m_vertWriter.create(bufferIndex);
}

ctnr::vltView_const<uint32_t> Resources::read_vertBuffers() const noexcept {
	return ctnr::vltView<uint32_t>{ m_vertBuffers }.with_static();
}

