#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct descriptorLayout_pAccess;
}

namespace rhi {

	class pcdDescriptorLayoutCreate;

	class pmvDescriptorLayout {
	public:
		pmvDescriptorLayout() noexcept;

		pmvDescriptorLayout(const pmvDescriptorLayout&) = delete;
		pmvDescriptorLayout& operator=(const pmvDescriptorLayout&) = delete;

		pmvDescriptorLayout(pmvDescriptorLayout&&) noexcept;
		pmvDescriptorLayout& operator=(pmvDescriptorLayout&&) noexcept;

		~pmvDescriptorLayout() noexcept;

	private:
		friend class pcdDescriptorLayoutCreate;
		friend struct rhi::access::descriptorLayout_pAccess;

		VkDescriptorSetLayout m_setLayout;

		VkDevice r_device;

	};

}