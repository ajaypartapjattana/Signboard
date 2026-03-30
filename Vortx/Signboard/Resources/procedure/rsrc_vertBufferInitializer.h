#pragma once

#include "Signboard/Core/Containers/storage.h"

namespace rhi {
	class pmvVertexLayout;
}

namespace rsrc {

	class pcdVertBuffersCreation {
	public:
		pcdVertBuffersCreation(const storage::vault_readAccessor<rhi::pmvVertexLayout> readVertexLayout) noexcept;

	private:
		storage::vault_readAccessor<rhi::pmvVertexLayout> m_readVertexLayout;

	};

}

