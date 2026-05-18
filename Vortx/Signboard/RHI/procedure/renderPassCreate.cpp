#include "renderPassCreate.h"

#include "Signboard/RHI/Internal/rhi_pAccess.h"

namespace rhi {

	pcdRenderPassCreate::pcdRenderPassCreate(const creDevice& device, VkRenderPassCreateInfo* pCreateInfo) noexcept
		: 
		r_device(_pAccess::extract(device)),
		pInfo(allot_basic(pCreateInfo))
	{

	}

	VkRenderPassCreateInfo* pcdRenderPassCreate::allot_basic(VkRenderPassCreateInfo* pCreateInfo) noexcept {
		if (pCreateInfo)
			return pCreateInfo;

		m_ownedInfo = std::make_unique<VkRenderPassCreateInfo>();

		VkRenderPassCreateInfo* _info = m_ownedInfo.get();
		_info->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

		return _info;
	}

	void pcdRenderPassCreate::target_attachments(sgb::span<const VkAttachmentDescription> attachments) noexcept {
		if (attachments.empty())
			return;
		
		pInfo->pAttachments = attachments.data();
		pInfo->attachmentCount = static_cast<uint32_t>(attachments.size());
	}

	uint32_t pcdRenderPassCreate::issue_subpass(VkPipelineBindPoint bindPoint) noexcept {
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = bindPoint;

		uint32_t _idx = static_cast<uint32_t>(m_subpasses.size());

		m_subpasses.emplace_back(subpass);

		return _idx;
	}

	void pcdRenderPassCreate::refer_colorAttachments(uint32_t subpassIndex, sgb::span<const VkAttachmentReference> references) noexcept {
		if (references.empty() || subpassIndex > m_subpasses.size() - 1)
			return;

		m_subpasses[subpassIndex].pColorAttachments = references.data();
		m_subpasses[subpassIndex].colorAttachmentCount = static_cast<uint32_t>(references.size());
	}

	void pcdRenderPassCreate::refer_depthStencilAttachment(uint32_t subpassIndex, VkAttachmentReference& reference) noexcept {
		if (subpassIndex > m_subpasses.size() - 1)
			return;

		m_subpasses[subpassIndex].pDepthStencilAttachment = &reference;
	}

	void pcdRenderPassCreate::target_dependancies(sgb::span<const VkSubpassDependency> dependancies) noexcept {
		if (dependancies.empty())
			return;

		pInfo->pDependencies = dependancies.data();
		pInfo->dependencyCount = static_cast<uint32_t>(dependancies.size());
	}

	VkResult pcdRenderPassCreate::publish(pmvRenderPass& target) noexcept {
		if (m_subpasses.empty())
			return VK_INCOMPLETE;

		pInfo->subpassCount = static_cast<uint32_t>(m_subpasses.size());
		pInfo->pSubpasses = m_subpasses.data();

		VkRenderPass _renderPass;
		VkResult result = vkCreateRenderPass(r_device, pInfo, nullptr, &_renderPass);
		if (result != VK_SUCCESS)
			return result;

		if (target.r_device)
			vkDestroyRenderPass(target.r_device, target.m_renderPass, nullptr);

		target.attachmentCount = pInfo->attachmentCount;
		target.r_device = r_device;

		return VK_SUCCESS;
	}

	void pcdRenderPassCreate::preset(VkRenderPassCreateInfo* pCreateInfo) noexcept {
		m_subpasses.clear();

		m_ownedInfo.reset();
		pInfo = allot_basic(pCreateInfo);
	}

	void pcdRenderPassCreate::reset() noexcept {
		m_subpasses.clear();

		m_ownedInfo.reset();
		pInfo = allot_basic(nullptr);
	}

}
