#pragma once

#include <vulkan/vulkan.h>
#include <memory>

namespace rhi {

	class creDevice;

	class pmvImage;

	class pcdImageViewCreate {
	public:
		pcdImageViewCreate(const creDevice& device, VkImageViewCreateInfo* pCreateInfo = nullptr) noexcept;

		void aspect(VkImageAspectFlags aspect) noexcept;
		void mipLevel(uint32_t baseLevel, uint32_t levelCount) noexcept;
		void arrayLayer(uint32_t baseLayer, uint32_t layerCount) noexcept;

		void components(VkComponentMapping mapping) noexcept;

		void target_image(const pmvImage& image) noexcept;

		VkResult publish(pmvImage& image) const noexcept;

	private:
		VkImageViewCreateInfo* allot_basic(VkImageViewCreateInfo* pCreateinfo) noexcept;

	private:
		const VkDevice r_device;

		std::unique_ptr<VkImageViewCreateInfo> m_ownedInfo;
		VkImageViewCreateInfo* pInfo;

	};

}