#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class creDevice;

	class pmvDescriptorSetLayout;

	class pcdDescriptorSetLayoutCreate {
	public:
		pcdDescriptorSetLayoutCreate(const creDevice& device, VkDescriptorSetLayoutCreateInfo* pCreateInfo = nullptr) noexcept;

		struct binding {
			uint32_t index;
			VkDescriptorType type;
			uint32_t count;
			VkPipelineStageFlags stage;
		};

		pcdDescriptorSetLayoutCreate& target_bindings(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

		VkResult publish(rhi::pmvDescriptorSetLayout& descriptorLayout) const noexcept;

		void preset(VkDescriptorSetLayoutCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkDescriptorSetLayoutCreateInfo fetch_basic(VkDescriptorSetLayoutCreateInfo* pCreateInfo) const noexcept;

	private:
		VkDevice r_device;

		VkDescriptorSetLayoutCreateInfo _info;

	};

}