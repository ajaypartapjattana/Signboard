#include "descriptorLayoutCreate.h"

#include "Signboard/RHI/primitive/descriptorLayout.h"

#include "Signboard/RHI/core/device_pAccess.h"

namespace rhi {

	pcdDescriptorLayoutCreate::pcdDescriptorLayoutCreate(const creDevice& device) noexcept 
		:
		r_device(rhi::access::device_pAccess::get(device)),
		_info()
	{
		_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	}

	void pcdDescriptorLayoutCreate::push_bindings(const std::vector<binding>& _binds) {
		uint32_t _bCt = static_cast<uint32_t>(_binds.size());
		bindings.reserve(_bCt);

		for (const binding& b : _binds) {
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = b.index;
			binding.descriptorType = b.type;
			binding.descriptorCount = b.count;
			binding.stageFlags = b.stage;

			bindings.push_back(binding);
		}
	}

	VkResult pcdDescriptorLayoutCreate::create(rhi::pmvDescriptorLayout& dl) {
		if (bindings.empty())
			return VK_INCOMPLETE;

		_info.pBindings = bindings.data();
		_info.bindingCount = static_cast<uint32_t>(bindings.size());

		VkDescriptorSetLayout _dsl;
		VkResult result = vkCreateDescriptorSetLayout(r_device, &_info, nullptr, &_dsl);
		if (result != VK_SUCCESS)
			return result;

		dl.m_setLayout = _dsl;
		dl.r_device = r_device;

		return VK_SUCCESS;
	}

}