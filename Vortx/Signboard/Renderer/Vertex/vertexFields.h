#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Core/Containers/storage/storage.h"

class vertexFields {
public:
	vertexFields() noexcept;

	vertexFields(const vertexFields&) = delete;
	vertexFields& operator=(const vertexFields&) = delete;

	struct createInfo {
		std::vector<VkFormat> orderedFormats;
	};

	storage::storage_handle createVertexLayout(const createInfo& info);

	storage::vault_readAccessor<rhi::pmvVertexLayout> get_vertexLayoutReadAccess() const noexcept;

private:
	storage::vault<rhi::pmvVertexLayout> m_vertexLayouts;
	storage::vault_writeAccessor<rhi::pmvVertexLayout> m_writer;

};