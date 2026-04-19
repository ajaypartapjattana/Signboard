#pragma once

#include <vulkan/vulkan.h>
#include "Signboard/Core/Containers/storage.h"

namespace rhi {

	class creDevice;

	class pmvRenderPass;
	class pmvImage;

	class pcdRenderPassCreate {
	public:
		pcdRenderPassCreate(const rhi::creDevice& device, VkRenderPassCreateInfo* pCreateInfo = nullptr) noexcept;

		pcdRenderPassCreate(const pcdRenderPassCreate&) = delete;
		pcdRenderPassCreate& operator=(const pcdRenderPassCreate&) = delete;

		void target_attachments(ctnr::span<const VkAttachmentDescription> attachments);

		_NODISCARD uint32_t issue_subpass(VkPipelineBindPoint pipelineBindPoint);

		void refer_colorAttachments(uint32_t subpass, ctnr::span<const VkAttachmentReference> references);
		void refer_depthStencilAttachment(uint32_t subpass, VkAttachmentReference& reference);

		void target_dependancies(ctnr::span<const VkSubpassDependency> dependancies);

		VkResult publish(rhi::pmvRenderPass& renderPass);

		void preset(VkRenderPassCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkRenderPassCreateInfo fetch_basic(VkRenderPassCreateInfo* pCreateInfo) const noexcept;

	private:
		VkDevice r_device;

		std::vector<VkSubpassDescription> m_subpasses;

		VkRenderPassCreateInfo _info;

	};

}