#include "descriptorLayoutCreate.h"

#include "Signboard/RHI/primitive/descriptorLayout.h"

#include "Signboard/RHI/core/device_pAccess.h"

namespace rhi {

	pcdDescriptorSetLayoutCreate::pcdDescriptorSetLayoutCreate(const creDevice& device, VkDescriptorSetLayoutCreateInfo* pCreateInfo) noexcept 
		:
		r_device(rhi::access::device_pAccess::extract(device)),
		_info(fetch_basic(pCreateInfo))
	{

	}

	VkDescriptorSetLayoutCreateInfo pcdDescriptorSetLayoutCreate::fetch_basic(VkDescriptorSetLayoutCreateInfo* pCreateInfo) const noexcept {
		if (pCreateInfo)
			return *pCreateInfo;

		VkDescriptorSetLayoutCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

		return info;
	}


	pcdDescriptorSetLayoutCreate& pcdDescriptorSetLayoutCreate::target_bindings(const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
		if (bindings.empty())
			return *this;
		
		_info.pBindings = bindings.data();
		_info.bindingCount = static_cast<uint32_t>(bindings.size());

		return *this;
	}

	VkResult pcdDescriptorSetLayoutCreate::publish(rhi::pmvDescriptorSetLayout& target) const noexcept {
		if (_info.bindingCount <= 0)
			return VK_INCOMPLETE;

		VkResult result = vkCreateDescriptorSetLayout(r_device, &_info, nullptr, &target.m_setLayout);
		if (result != VK_SUCCESS)
			return result;

		target.r_device = r_device;

		return VK_SUCCESS;
	}

	void pcdDescriptorSetLayoutCreate::preset(VkDescriptorSetLayoutCreateInfo* pCreateInfo) noexcept {
		_info = fetch_basic(pCreateInfo);
	}

	void pcdDescriptorSetLayoutCreate::reset() noexcept {
		_info = fetch_basic(nullptr);
	}

}