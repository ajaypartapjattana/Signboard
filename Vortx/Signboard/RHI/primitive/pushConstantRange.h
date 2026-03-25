#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct pushConstantRange_pAccess;
}

namespace rhi {

	class pcdPushConstantRangeCreator;

	class pmvPushConstantRange {
	public:
		pmvPushConstantRange() noexcept;

		pmvPushConstantRange(const pmvPushConstantRange&) = delete;
		pmvPushConstantRange& operator=(const pmvPushConstantRange&) = delete;

		pmvPushConstantRange(pmvPushConstantRange&&) noexcept;
		pmvPushConstantRange& operator=(pmvPushConstantRange&&) noexcept;

		~pmvPushConstantRange() noexcept;

	private:
		friend class pcdPushConstantRangeCreator;
		friend rhi::access::pushConstantRange_pAccess;

		VkPushConstantRange m_range;

	};

}