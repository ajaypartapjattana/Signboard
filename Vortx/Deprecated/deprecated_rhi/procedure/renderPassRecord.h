#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "Signboard/Core/core.h"

namespace rhi {

	class pmvCommandbuffer;

	class pcdRenderPassRecord {
	public:
		pcdRenderPassRecord(VkRenderPassBeginInfo* pbeginInfo = nullptr) noexcept;

		void target_commandBuffer(const pmvCommandBuffer& commandbuffer) noexcept;

		void target_clearValues(sgb::span<const VkClearValue> clearValues);
		void renderArea(VkRect2D rect) noexcept;
		void begin(const pmvRenderPass& renderPass, const pmvFramebuffer& framebuffer) noexcept;

		void bind_pipeline(const pmvPipeline& pipeline) const noexcept;
		void bind_vertexBuffer(sgb::span<const pmvBuffer*> vertexBuffers, sgb::span<const VkDeviceSize> offsets) const noexcept;
		void bind_indexBuffer(const pmvBuffer& buffer) const noexcept;

		void viewport(sgb::span<const VkViewport> viewports, sgb::span<const VkRect2D> scissors) const noexcept;
		void draw(uint32_t indexCount) const noexcept;
		
		void end() const noexcept;

	private:
		VkRenderPassBeginInfo* allot_basic(VkRenderPassBeginInfo* pBeginInfo) noexcept;

	private:
		VkCommandBuffer r_commandBuffer;

		std::unique_ptr<VkRenderPassBeginInfo> m_ownedInfo;
		VkRenderPassBeginInfo* pInfo;

	};


}