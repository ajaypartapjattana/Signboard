#include "renderPassBuilder.h"

#include "Signboard/RHI/primitive/renderPass.h"
#include "Signboard/RHI/core/device_pAccess.h"

namespace rhi {

	pcdRenderPassCreate::pcdRenderPassCreate(const rhi::creDevice& device, VkRenderPassCreateInfo* pCreateInfo) noexcept
		: 
		r_device(rhi::access::device_pAccess::get(device)),
		_info(fetch_basic(pCreateInfo))
	{

	}

	VkRenderPassCreateInfo pcdRenderPassCreate::fetch_basic(VkRenderPassCreateInfo* pCreateInfo) const noexcept {
		if (pCreateInfo)
			return *pCreateInfo;

		VkRenderPassCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

		return info;
	}

	void pcdRenderPassCreate::target_attachments(ctnr::span<const VkAttachmentDescription> attachments) {
		if (attachments.empty())
			return;
		
		_info.pAttachments = attachments.data();
		_info.attachmentCount = static_cast<uint32_t>(attachments.size());
		
		return;
	}

	uint32_t pcdRenderPassCreate::issue_subpass(VkPipelineBindPoint bindPoint) {
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = bindPoint;

		m_subpasses.emplace_back(subpass);

		return m_subpasses.size() - 1;
	}

	void pcdRenderPassCreate::refer_colorAttachments(uint32_t subpassIndex, ctnr::span<const VkAttachmentReference> references) {
		if (references.empty() || subpassIndex > m_subpasses.size() - 1)
			return;

		m_subpasses[subpassIndex].pColorAttachments = references.data();
		m_subpasses[subpassIndex].colorAttachmentCount = static_cast<uint32_t>(references.size());
	}

	void pcdRenderPassCreate::refer_depthStencilAttachment(uint32_t subpassIndex, VkAttachmentReference& reference) {
		if (subpassIndex > m_subpasses.size() - 1)
			return;

		m_subpasses[subpassIndex].pDepthStencilAttachment = &reference;
	}

	void pcdRenderPassCreate::target_dependancies(ctnr::span<const VkSubpassDependency> dependancies) {
		if (dependancies.empty())
			return;

		_info.pDependencies = dependancies.data();
		_info.dependencyCount = static_cast<uint32_t>(dependancies.size());
	}

	VkResult pcdRenderPassCreate::publish(rhi::pmvRenderPass& target) {
		if (m_subpasses.empty())
			return VK_INCOMPLETE;

		_info.subpassCount = static_cast<uint32_t>(m_subpasses.size());
		_info.pSubpasses = m_subpasses.data();

		VkResult result = vkCreateRenderPass(r_device, &_info, nullptr, &target.m_renderPass);
		if (result != VK_SUCCESS)
			return result;

		target.ordered_attachmentFormats.reserve(_info.attachmentCount);
		for (uint32_t i = 0; i < _info.attachmentCount; ++i) {
			target.ordered_attachmentFormats.push_back(_info.pAttachments[i].format);
		}

		target.r_device = r_device;

		return VK_SUCCESS;
	}

	void pcdRenderPassCreate::preset(VkRenderPassCreateInfo* pCreateInfo) noexcept {
		_info = fetch_basic(pCreateInfo);
	}

	void pcdRenderPassCreate::reset() noexcept {
		m_subpasses.clear();
		_info = fetch_basic(nullptr);
	}

}
