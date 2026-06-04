#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "Signboard/Core/core.h"

namespace rhi {

	class device;

	class pmvRenderPass;
	class pmvImage;

	class pcdRenderPassCreate {
	public:
		pcdRenderPassCreate(const rhi::device& device, VkRenderPassCreateInfo* pCreateInfo = nullptr) noexcept;

		pcdRenderPassCreate(const pcdRenderPassCreate&) = delete;
		pcdRenderPassCreate& operator=(const pcdRenderPassCreate&) = delete;

		void target_attachments(sgb::span<const VkAttachmentDescription> attachments) noexcept;

		_NODISCARD uint32_t issue_subpass(VkPipelineBindPoint pipelineBindPoint) noexcept;

		void refer_colorAttachments(uint32_t subpass, sgb::span<const VkAttachmentReference> references) noexcept;
		void refer_depthStencilAttachment(uint32_t subpass, VkAttachmentReference& reference) noexcept;

		void target_dependancies(sgb::span<const VkSubpassDependency> dependancies) noexcept;

		VkResult publish(rhi::pmvRenderPass& renderPass) noexcept;

		void preset(VkRenderPassCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkRenderPassCreateInfo* allot_basic(VkRenderPassCreateInfo* pCreateInfo) noexcept;

	private:
		VkDevice r_device;

		std::vector<VkSubpassDescription> m_subpasses;

		std::unique_ptr<VkRenderPassCreateInfo> m_ownedInfo;
		VkRenderPassCreateInfo* pInfo;

	};

}