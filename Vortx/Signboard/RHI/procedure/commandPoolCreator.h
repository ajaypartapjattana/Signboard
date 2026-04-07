#pragma once

#include "Signboard/RHI/detail/standardCommandPools.h"

namespace rhi {

	class creDevice;

	class pcdCommandPoolCreator {
	public:
		pcdCommandPoolCreator(const rhi::creDevice& device) noexcept;

		VkResult create(standardCommandPools& commandPools);

	private:
		const rhi::creDevice& _dvc;

	};

}
