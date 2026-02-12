#pragma once

namespace rhi::procedure {
	class descriptorLayout_creator;
}

#include <vulkan/vulkan.h>

namespace rhi::primitive {

	struct descriptorLayout_vkAccess;

	class descriptorLayout {
	public:
		descriptorLayout() noexcept;

		descriptorLayout(const descriptorLayout&) = delete;
		descriptorLayout& operator=(const descriptorLayout&) = delete;

		descriptorLayout(descriptorLayout&&) noexcept;
		descriptorLayout& operator=(descriptorLayout&&) noexcept;

		~descriptorLayout() noexcept;

		VkDescriptorSetLayout native_setLayout() const noexcept;

	private:
		friend class rhi::procedure::descriptorLayout_creator;
		friend struct descriptorLayout_vkAccess;

		VkDescriptorSetLayout m_setLayout;

		VkDevice m_device;

	};

}