#include "rsrc_buffers.h"

rsrc_buffers::rsrc_buffers(const rhi::creAllocator& allocator)
	:
	_allctr(allocator),

	m_writer(m_buffers)
{

}

_NODISCARD uint32_t rsrc_buffers::createBuffer(const createInfo& info) noexcept {
	_allctr.addUsage(info.usage);
	_allctr.setMemoryPreference(info.memory);
	_allctr.setBufferSize(info.size);
	_allctr.setMemoryFlags(info.allocationFlags);

	auto builder = [&](rhi::pmvBuffer* b) {
		_allctr.allocateBuffer(*b);
	};

	return m_writer.construct(builder);
}

ctnr::vltView<rhi::pmvBuffer> rsrc_buffers::read_buffers() const noexcept {
	return ctnr::vltView<rhi::pmvBuffer>{ m_buffers };
}
