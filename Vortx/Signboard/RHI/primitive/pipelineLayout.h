#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	struct _pAccess;

	class pcdPipelineLayoutCreate;

	class pmvPipelineLayout {
	public:
		pmvPipelineLayout() noexcept;

		pmvPipelineLayout(const pmvPipelineLayout&) = delete;
		pmvPipelineLayout& operator=(const pmvPipelineLayout&) = delete;

		pmvPipelineLayout(pmvPipelineLayout&&) noexcept;
		pmvPipelineLayout& operator=(pmvPipelineLayout&&) noexcept;

		~pmvPipelineLayout() noexcept;

		void reset() noexcept;

	private:
		friend class pcdPipelineLayoutCreate;
		friend struct _pAccess;

		VkPipelineLayout m_layout;

		VkDevice r_device;

	};

}