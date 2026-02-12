#pragma once

namespace rhi::procedure {
	class renderPass_builder;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {

	struct renderPass_vkAccess;

	class renderPass {
	public:
		renderPass() noexcept;

		renderPass(const renderPass&) = delete;
		renderPass& operator=(const renderPass&) = delete;

		renderPass(renderPass&&) noexcept;
		renderPass& operator=(renderPass&&) noexcept;

		~renderPass() noexcept;

		VkRenderPass native_renderPass() const noexcept;

	private:
		friend class rhi::procedure::renderPass_builder;
		friend struct renderPass_vkAccess;

		VkRenderPass m_renderPass;

		VkDevice m_device;

	};

}