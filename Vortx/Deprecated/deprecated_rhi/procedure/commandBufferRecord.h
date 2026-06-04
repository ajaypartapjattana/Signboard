#pragma once

#include "Signboard/Core/core.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace rhi {

	class pmvCommandBuffer;
	class pmvBuffer;

	class pcdCommandBufferRecord {
	public:
		pcdCommandBufferRecord(VkCommandBufferBeginInfo* pBeginInfo = nullptr) noexcept;
		
		void target_buffer(const pmvCommandBuffer& commandBuffer) noexcept;
		void reset_buffer(VkCommandBufferResetFlags flags) const noexcept;
		
		VkResult begin() const;

		void uploadBuffer(const pmvBuffer& src, const pmvBuffer& dst, sgb::span<const VkBufferCopy> copyInfo) const;

		VkResult end() const noexcept;


	private:
		VkCommandBufferBeginInfo* allot_basic(VkCommandBufferBeginInfo* pBeginInfo) noexcept;

	private:
		VkCommandBuffer r_commandBuffer;

		std::unique_ptr<VkCommandBufferBeginInfo> m_ownedBeginInfo;
		VkCommandBufferBeginInfo* pBeginInfo;

	};

}