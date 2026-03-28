#include "rsrc_buffers.h"

#include "Signboard/Renderer/RenderBackend/rndr_context_Access.h"

rsrc_buffers::rsrc_buffers(const rndr_context& context) 
	:
	r_allocator(rndr_context_Access::get_allocator(context)),

	m_writer(m_buffers)
{

}

void rsrc_buffers::createBuffer(const createInfo& info) {
	rhi::pcdBufferAllocator prcdr{ r_allocator };

	prcdr.addUsage(info.usage);
	prcdr.setMemoryPreference(VMA_MEMORY_USAGE_AUTO);
	prcdr.setBufferSize(info.vertexLayout->stride() * info.vertexCount);

	auto builder = [&](rhi::pmvBuffer* b) {
		prcdr.allocateBuffer(*b);
	};

	m_writer.construct(builder);
}

storage::vault_readAccessor<rhi::pmvBuffer> rsrc_buffers::get_bufferReadAccess() const noexcept {
	return storage::vault_readAccessor<rhi::pmvBuffer>{m_buffers};
}
