#pragma once

namespace rhi::procedure {
	class renderPass_builder;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {

	struct renderPass_vkAccess;

	class renderPass {
	public:
		renderPass(const renderPass&) = delete;
		renderPass& operator=(const renderPass&) = delete;

		renderPass(renderPass&&) noexcept;
		renderPass& operator=(renderPass&&) noexcept;

		~renderPass() noexcept;

		VkRenderPass native_renderPass() const noexcept;

	private:
		friend class rhi::procedure::renderPass_builder;
		friend struct renderPass_vkAccess;

		renderPass() = default;

		VkRenderPass m_renderPass;

		VkDevice m_device;

	};

}