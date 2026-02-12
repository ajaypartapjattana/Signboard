#pragma once

namespace rhi::procedure {
	class pushConstantRange_creator;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {

	struct pushConstantRange_vkAccess;

	class pushConstantRange {
	public:
		pushConstantRange() noexcept;

		pushConstantRange(const pushConstantRange&) = delete;
		pushConstantRange& operator=(const pushConstantRange&) = delete;

		pushConstantRange(pushConstantRange&&) noexcept;
		pushConstantRange& operator=(pushConstantRange&&) noexcept;

		~pushConstantRange() noexcept;

		VkPushConstantRange native_pushConstantRange() const noexcept;

	private:
		friend class rhi::procedure::pushConstantRange_creator;
		friend pushConstantRange_vkAccess;

		VkPushConstantRange m_range;

	};

}