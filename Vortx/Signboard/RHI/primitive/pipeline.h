#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	struct _pAccess;

	class pcdGraphicPipelineCreate;

	class pmvPipeline {
	public:
		pmvPipeline() noexcept;

		pmvPipeline(const pmvPipeline&) = delete;
		pmvPipeline& operator=(const pmvPipeline&) = delete;

		pmvPipeline(pmvPipeline&&) noexcept;
		pmvPipeline& operator=(pmvPipeline&&) noexcept;

		~pmvPipeline() noexcept;

		void reset() noexcept;

	private:
		friend class pcdGraphicPipelineCreate;
		friend struct _pAccess;

		VkPipeline m_pipeline;
		VkPipelineBindPoint type;

		VkDevice r_device;

	};

}