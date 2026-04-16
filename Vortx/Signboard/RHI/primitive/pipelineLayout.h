#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct pipelineLayout_pAccess;
}

namespace rhi {

	class pcdPipelineLayoutCreate;

	class pmvPipelineLayout {
	public:
		pmvPipelineLayout() noexcept;

		pmvPipelineLayout(const pmvPipelineLayout&) = delete;
		pmvPipelineLayout& operator=(const pmvPipelineLayout&) = delete;

		pmvPipelineLayout(pmvPipelineLayout&&) noexcept;
		pmvPipelineLayout& operator=(pmvPipelineLayout&&) noexcept;

		~pmvPipelineLayout() noexcept;

	private:
		friend class pcdPipelineLayoutCreate;
		friend struct rhi::access::pipelineLayout_pAccess;

		VkPipelineLayout m_layout;

		VkDevice _dvc;

	};

}