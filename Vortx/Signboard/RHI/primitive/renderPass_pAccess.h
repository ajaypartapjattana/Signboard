#pragma once

#include "renderPass.h"
#include "Signboard/Core/Containers/storage.h"

namespace rhi::access {

	struct renderPass_pAccess {

		static VkRenderPass extract(const rhi::pmvRenderPass& rp) noexcept {
			return rp.m_renderPass;
		}

		static ctnr::span<const VkFormat> get_orderedAttachmentFormats(const rhi::pmvRenderPass& rp) noexcept {
			return rp.ordered_attachmentFormats;
		}

	};

}