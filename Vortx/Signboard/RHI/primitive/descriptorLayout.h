#pragma once

#include <vulkan/vulkan.h>

namespace rhi::access {
	struct descriptorSetLayout_pAccess;
}

namespace rhi {

	class pcdDescriptorSetLayoutCreate;

	class pmvDescriptorSetLayout {
	public:
		pmvDescriptorSetLayout() noexcept;

		pmvDescriptorSetLayout(const pmvDescriptorSetLayout&) = delete;
		pmvDescriptorSetLayout& operator=(const pmvDescriptorSetLayout&) = delete;

		pmvDescriptorSetLayout(pmvDescriptorSetLayout&&) noexcept;
		pmvDescriptorSetLayout& operator=(pmvDescriptorSetLayout&&) noexcept;

		~pmvDescriptorSetLayout() noexcept;

	private:
		friend class pcdDescriptorSetLayoutCreate;
		friend struct rhi::access::descriptorSetLayout_pAccess;

		VkDescriptorSetLayout m_setLayout;

		VkDevice r_device;

	};

}