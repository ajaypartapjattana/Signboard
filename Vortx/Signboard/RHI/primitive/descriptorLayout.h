#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	struct _pAccess;

	class pcdDescriptorSetLayoutCreate;

	class pmvDescriptorSetLayout {
	public:
		pmvDescriptorSetLayout() noexcept;

		pmvDescriptorSetLayout(const pmvDescriptorSetLayout&) noexcept;
		pmvDescriptorSetLayout& operator=(const pmvDescriptorSetLayout&) noexcept;

		pmvDescriptorSetLayout(pmvDescriptorSetLayout&&) noexcept;
		pmvDescriptorSetLayout& operator=(pmvDescriptorSetLayout&&) noexcept;

		~pmvDescriptorSetLayout() noexcept;

		void reset() noexcept;

	private:
		friend class pcdDescriptorSetLayoutCreate;
		friend struct _pAccess;

		VkDescriptorSetLayout m_setLayout;

		VkDevice r_device;

	};

}