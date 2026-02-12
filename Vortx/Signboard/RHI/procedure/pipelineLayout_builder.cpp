#include "pipelineLayout_builder.h"

#include "Signboard/RHI/primitive/pipelineLayout.h"
#include "Signboard/RHI/core/device_vk.h"
#include "Signboard/RHI/primitive/descriptorLayout_vk.h"
#include "Signboard/RHI/primitive/pushConstantRange_vk.h"

namespace rhi::procedure{

	pipelineLayout_builder::pipelineLayout_builder(const rhi::core::device& device)
		: m_device(rhi::core::device_vkAccess::get(device))
	{

	}

	pipelineLayout_builder& pipelineLayout_builder::add_setLayout(const rhi::primitive::descriptorLayout& dl) {
		VkDescriptorSetLayout vk_setLayout = rhi::primitive::descriptorLayout_vkAccess::get(dl);
		m_setLayouts.push_back(vk_setLayout);

		return *this;
	}

	pipelineLayout_builder& pipelineLayout_builder::add_pushConstantRange(const rhi::primitive::pushConstantRange& pcr) {
		VkPushConstantRange vk_pushConstantRange = rhi::primitive::pushConstantRange_vkAccess::get(pcr);
		m_pushConstantRanges.push_back(vk_pushConstantRange);

		return *this;
	}

	VkResult pipelineLayout_builder::build(rhi::primitive::pipelineLayout& tw_pipelineLayout) {
		if (m_setLayouts.empty() && m_pushConstantRanges.empty())
			return VK_INCOMPLETE;

		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = static_cast<uint32_t>(m_setLayouts.size());
		layoutInfo.pSetLayouts = m_setLayouts.data();
		layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(m_pushConstantRanges.size());
		layoutInfo.pPushConstantRanges = m_pushConstantRanges.data();

		VkPipelineLayout vk_layout = VK_NULL_HANDLE;
		VkResult result =  vkCreatePipelineLayout(m_device, &layoutInfo, nullptr, &vk_layout);

		tw_pipelineLayout.m_layout = vk_layout;
		tw_pipelineLayout.m_device = m_device;

		return result;
	}

}