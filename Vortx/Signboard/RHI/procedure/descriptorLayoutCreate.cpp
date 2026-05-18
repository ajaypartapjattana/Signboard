#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdDescriptorSetLayoutCreate::pcdDescriptorSetLayoutCreate(const creDevice& device, VkDescriptorSetLayoutCreateInfo* pCreateInfo) noexcept 
		:
		r_device(_pAccess::extract(device)),
		pInfo(allot_basic(pCreateInfo))
	{

	}

	VkDescriptorSetLayoutCreateInfo* pcdDescriptorSetLayoutCreate::allot_basic(VkDescriptorSetLayoutCreateInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkDescriptorSetLayoutCreateInfo>();

		VkDescriptorSetLayoutCreateInfo* _info = m_ownedInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

		return _info;
	}


	void pcdDescriptorSetLayoutCreate::target_bindings(sgb::span<const VkDescriptorSetLayoutBinding> bindings) {
		if (bindings.empty())
			return;
		
		pInfo->pBindings = bindings.data();
		pInfo->bindingCount = static_cast<uint32_t>(bindings.size());
	}

	VkResult pcdDescriptorSetLayoutCreate::publish(pmvDescriptorSetLayout& target) const noexcept {
		VkDescriptorSetLayout _setLayout;
		VkResult result = vkCreateDescriptorSetLayout(r_device, pInfo, nullptr, &_setLayout);
		if (result != VK_SUCCESS)
			return result;

		target.reset();

		target.m_setLayout = _setLayout;
		target.r_device = r_device;

		return VK_SUCCESS;
	}

	void pcdDescriptorSetLayoutCreate::preset(VkDescriptorSetLayoutCreateInfo* pCreateInfo) noexcept {
		m_ownedInfo.reset();
		pInfo = allot_basic(pCreateInfo);
	}

	void pcdDescriptorSetLayoutCreate::reset() noexcept {
		m_ownedInfo.reset();
		pInfo = allot_basic(nullptr);
	}

}