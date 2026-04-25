#include "pipelineLayoutBuilder.h"

#include "Signboard/RHI/primitive/pipelineLayout.h"

#include "Signboard/RHI/core/device_pAccess.h"
#include "Signboard/RHI/primitive/descriptorLayout_pAccess.h"

namespace rhi{

	pcdPipelineLayoutCreate::pcdPipelineLayoutCreate(const rhi::creDevice& device, VkPipelineLayoutCreateInfo* pCreateInfo)
		: 
		r_device(rhi::access::device_pAccess::extract(device)),
		_info(fetch_basic(pCreateInfo))
	{

	}

	VkPipelineLayoutCreateInfo pcdPipelineLayoutCreate::fetch_basic(VkPipelineLayoutCreateInfo* pCreateInfo) const noexcept {
		if (pCreateInfo)
			return *pCreateInfo;

		VkPipelineLayoutCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		return info;
	}

	pcdPipelineLayoutCreate& pcdPipelineLayoutCreate::push_descriptorSetLayouts(const rhi::pmvDescriptorSetLayout& dl) {
		VkDescriptorSetLayout _layout = rhi::access::descriptorSetLayout_pAccess::extract(dl);
		m_descriptorSetLayouts.push_back(_layout);

		return *this;
	}

	pcdPipelineLayoutCreate& pcdPipelineLayoutCreate::push_descriptorSetLayouts(const std::vector<rhi::pmvDescriptorSetLayout>& layouts) {
		m_descriptorSetLayouts.reserve(m_descriptorSetLayouts.size() + layouts.size());

		for (const rhi::pmvDescriptorSetLayout& l : layouts) {
			m_descriptorSetLayouts.push_back(rhi::access::descriptorSetLayout_pAccess::extract(l));
		}
		
		return *this;
	}

	pcdPipelineLayoutCreate& pcdPipelineLayoutCreate::target_pushConstantRanges(const std::vector<VkPushConstantRange>& ranges) {
		_info.pPushConstantRanges = ranges.data();
		_info.pushConstantRangeCount = static_cast<uint32_t>(ranges.size());

		return *this;
	}

	VkResult pcdPipelineLayoutCreate::publish(rhi::pmvPipelineLayout& target) {
		_info.pSetLayouts = m_descriptorSetLayouts.data();
		_info.setLayoutCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());

		VkResult result =  vkCreatePipelineLayout(r_device, &_info, nullptr, &target.m_layout);
		if (result != VK_SUCCESS)
			return result;

		target.r_device = r_device;

		return result;
	}

	void pcdPipelineLayoutCreate::preset(VkPipelineLayoutCreateInfo* pCreateInfo) noexcept {
		_info = fetch_basic(pCreateInfo);
	}

	void pcdPipelineLayoutCreate::reset() noexcept {
		m_descriptorSetLayouts.clear();
		_info = fetch_basic(nullptr);
	}

}