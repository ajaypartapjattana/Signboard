#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi{

	pcdPipelineLayoutCreate::pcdPipelineLayoutCreate(const creDevice& device, VkPipelineLayoutCreateInfo* pCreateInfo)
		: 
		r_device(_pAccess::extract(device)),
		pInfo(allot_basic(pCreateInfo))
	{

	}

	VkPipelineLayoutCreateInfo* pcdPipelineLayoutCreate::allot_basic(VkPipelineLayoutCreateInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkPipelineLayoutCreateInfo>();

		VkPipelineLayoutCreateInfo* _info = m_ownedInfo.get();

		_info->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		return _info;
	}

	void pcdPipelineLayoutCreate::target_descriptorSetLayouts(sgb::span<const pmvDescriptorSetLayout*> layouts) noexcept {
		size_t _lSz = layouts.size();

		m_setLayouts.resize(_lSz);
		for (size_t i = 0; i < _lSz; ++i) {
			m_setLayouts[i] = _pAccess::extract(*layouts[i]);
		}

		VkPipelineLayoutCreateInfo& info = *pInfo;

		info.pSetLayouts = m_setLayouts.data();
		info.setLayoutCount = static_cast<uint32_t>(m_setLayouts.size());
	}

	void pcdPipelineLayoutCreate::target_pushConstantRanges(sgb::span<const VkPushConstantRange> ranges) noexcept {
		VkPipelineLayoutCreateInfo& info = *pInfo;

		info.pPushConstantRanges = ranges.data();
		info.pushConstantRangeCount = static_cast<uint32_t>(ranges.size());
	}

	VkResult pcdPipelineLayoutCreate::publish(pmvPipelineLayout& target) const noexcept {
		VkPipelineLayout _layout;
		VkResult result =  vkCreatePipelineLayout(r_device, pInfo, nullptr, &_layout);
		if (result != VK_SUCCESS)
			return result;

		if (target.r_device)
			vkDestroyPipelineLayout(target.r_device, target.m_layout, nullptr);

		target.m_layout = _layout;
		target.r_device = r_device;

		return result;
	}

	void pcdPipelineLayoutCreate::preset(VkPipelineLayoutCreateInfo* pCreateInfo) noexcept {
		m_setLayouts.clear();
		pInfo = allot_basic(pCreateInfo);
	}

	void pcdPipelineLayoutCreate::reset() noexcept {
		m_setLayouts.clear();
		pInfo = allot_basic(nullptr);
	}

}