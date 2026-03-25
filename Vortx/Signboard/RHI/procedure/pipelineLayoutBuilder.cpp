#include "pipelineLayoutBuilder.h"

#include "Signboard/RHI/core/device_pAccess.h"

#include "Signboard/RHI/primitive/pipelineLayout.h"
#include "Signboard/RHI/primitive/descriptorLayout_pAccess.h"
#include "Signboard/RHI/primitive/pushConstantRange_pAccess.h"

namespace rhi{

	pcdPipelineLayoutBuilder::pcdPipelineLayoutBuilder(const rhi::creDevice& device)
		: m_device(rhi::access::device_pAccess::get(device))
	{

	}

	pcdPipelineLayoutBuilder& pcdPipelineLayoutBuilder::add_setLayout(const rhi::pmvDescriptorLayout& dl) {
		VkDescriptorSetLayout vk_setLayout = rhi::access::descriptorLayout_pAccess::get(dl);
		m_setLayouts.push_back(vk_setLayout);

		return *this;
	}

	pcdPipelineLayoutBuilder& pcdPipelineLayoutBuilder::add_pushConstantRange(const rhi::pmvPushConstantRange& range) {
		VkPushConstantRange vk_pushConstantRange = rhi::access::pushConstantRange_pAccess::get(range);
		m_pushConstantRanges.push_back(vk_pushConstantRange);

		return *this;
	}

	VkResult pcdPipelineLayoutBuilder::build(rhi::pmvPipelineLayout& tw_pipelineLayout) {

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