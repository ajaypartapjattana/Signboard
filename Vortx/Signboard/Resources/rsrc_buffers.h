#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/core/Containers/storage/storage.h"

class rndr_context;

class rsrc_buffers {
public:
	rsrc_buffers(const rndr_context& context);

	struct createInfo {
		VkBufferUsageFlags usage;

		size_t vertexCount;
		const rhi::pmvVertexLayout* vertexLayout;
	};

	void createBuffer(const createInfo& info);

	storage::vault_readAccessor<rhi::pmvBuffer> get_bufferReadAccess() const noexcept;

private:
	const rhi::creAllocator& r_allocator;

	storage::vault<rhi::pmvBuffer> m_buffers;
	storage::vault_writeAccessor<rhi::pmvBuffer> m_writer;

};