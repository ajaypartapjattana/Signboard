#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct pipeline_pAccess;
}

namespace rhi {

	class pcdPipelineBuilder;

	class pmvPipeline {
	public:
		pmvPipeline() noexcept;

		pmvPipeline(const pmvPipeline&) = delete;
		pmvPipeline& operator=(const pmvPipeline&) = delete;

		pmvPipeline(pmvPipeline&&) noexcept;
		pmvPipeline& operator=(pmvPipeline&&) noexcept;

		~pmvPipeline() noexcept;

	private:
		friend class pcdPipelineBuilder;
		friend struct rhi::access::pipeline_pAccess;

		VkPipeline m_pipeline;
		VkPipelineBindPoint m_type;

		VkDevice _dvc;

	};

}