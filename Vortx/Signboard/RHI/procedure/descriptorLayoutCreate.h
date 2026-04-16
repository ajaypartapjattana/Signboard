#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class creDevice;

	class pmvDescriptorLayout;

	class pcdDescriptorLayoutCreate {
	public:
		pcdDescriptorLayoutCreate(const creDevice& device) noexcept;

		struct binding {
			uint32_t index;
			VkDescriptorType type;
			uint32_t count;
			VkPipelineStageFlags stage;
		};

		void push_bindings(const std::vector<binding>& bindings);

		VkResult create(rhi::pmvDescriptorLayout& descriptorLayout);

	private:
		VkDevice r_device;

		std::vector<VkDescriptorSetLayoutBinding> bindings;
		VkDescriptorSetLayoutCreateInfo _info;

	};

}