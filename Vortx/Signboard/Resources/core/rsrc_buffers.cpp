#include "rsrc_buffers.h"

#include "Signboard/Renderer/RenderBackend/rndr_context_Access.h"

rsrc_buffers::rsrc_buffers(const rhi::creAllocator& allocator)
	:
	m_allocator(allocator),

	m_writer(m_buffers)
{

}

_NODISCARD uint32_t rsrc_buffers::createBuffer(const createInfo& info) noexcept {
	m_allocator.addUsage(info.usage);
	m_allocator.setMemoryPreference(info.memory);
	m_allocator.setBufferSize(info.size);
	m_allocator.setMemoryFlags(info.allocationFlags);

	auto builder = [&](rhi::pmvBuffer* b) {
		m_allocator.allocateBuffer(*b);
	};

	return m_writer.construct(builder);
}

void* rsrc_buffers::getbufferMapping(uint32_t handle) const {
	rhi::pmvBuffer* _buffer = m_writer.get(handle);
	return m_allocator.mapBuffer(*_buffer);
}

ctnr::vltView_const<rhi::pmvBuffer> rsrc_buffers::get_bufferReadAccess() const noexcept {
	return ctnr::vltView<rhi::pmvBuffer>{ m_buffers }.with_static();
}
