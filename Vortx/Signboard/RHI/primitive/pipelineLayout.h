#pragma once

namespace rhi::procedure {
	class pipelineLayout_builder;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {

	struct pipelineLayout_vkAccess;

	class pipelineLayout {
	public:
		pipelineLayout() noexcept;

		pipelineLayout(const pipelineLayout&) = delete;
		pipelineLayout& operator=(const pipelineLayout&) = delete;

		pipelineLayout(pipelineLayout&&) noexcept;
		pipelineLayout& operator=(pipelineLayout&&) noexcept;

		~pipelineLayout() noexcept;

		VkPipelineLayout native_layout() const noexcept;

	private:
		friend class rhi::procedure::pipelineLayout_builder;
		friend struct pipelineLayout_vkAccess;

		VkPipelineLayout m_layout;

		VkDevice m_device;

	};

}