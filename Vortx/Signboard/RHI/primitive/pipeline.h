#pragma once

namespace rhi::procedure {
	class pipeline_builder;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {

	struct pipeline_vkAccess;

	class pipeline {
	public:
		pipeline() noexcept;

		pipeline(const pipeline&) = delete;
		pipeline& operator=(const pipeline&) = delete;

		pipeline(pipeline&&) noexcept;
		pipeline& operator=(pipeline&&) noexcept;

		~pipeline() noexcept;

		VkPipeline native_pipeline() const noexcept;

	private:
		friend class rhi::procedure::pipeline_builder;
		friend struct pipeline_vkAccess;

		VkPipeline m_pipeline = VK_NULL_HANDLE;

		VkDevice m_device = VK_NULL_HANDLE;

	};

}