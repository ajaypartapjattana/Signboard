#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	class creDevice;

	class pmvImage;

	class pcdImageViewCreate {
	public:
		pcdImageViewCreate(const creDevice& device, const VkImageViewCreateInfo* pCreateInfo = nullptr) noexcept;

		void aspect(VkImageAspectFlags aspect) noexcept;
		void mipRange(uint32_t baseLevel, uint32_t levelCount) noexcept;
		void imageArray(uint32_t baseLayer, uint32_t layerCount) noexcept;

		void envelop_image(pmvImage& image) noexcept;

		VkResult publish(pmvImage& image);

	private:
		VkImageViewCreateInfo fetch_basic(const VkImageViewCreateInfo* pCreateinfo) const noexcept;

	private:
		const VkDevice r_device;

		VkImageViewCreateInfo _info;

	};

}