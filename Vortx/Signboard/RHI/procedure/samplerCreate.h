#pragma once

#include <vulkan/vulkan.h>
#include <memory>

namespace rhi {
	
	class creDevice;

	class pmvSampler;

	class pcdSamplerCreate {
	public:
		pcdSamplerCreate(const creDevice& device, VkSamplerCreateInfo* pCreateinfo = nullptr) noexcept;

		pcdSamplerCreate(const pcdSamplerCreate&) = delete;
		pcdSamplerCreate& operator=(const pcdSamplerCreate&) = delete;

		void addressing(VkSamplerAddressMode mode) noexcept;
		void filter(VkFilter filter, VkSamplerMipmapMode mode, VkBorderColor borderColor) noexcept;
		void Lod(float min, float max, float bias) noexcept;

		void enableAnisotropy(float maxAnisotropy) noexcept;
		void enableCompare(VkCompareOp Op) noexcept;
		void UnnormalizedCoordinates(bool enable) noexcept;

		VkResult publish(pmvSampler& sampler) const noexcept;

		void preset(VkSamplerCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkSamplerCreateInfo* allot_basic(VkSamplerCreateInfo* pCreateInfo) noexcept;

	private:
		const VkDevice r_device;
		VkBool32 enabled_anisotropy;
		
		std::unique_ptr<VkSamplerCreateInfo> m_ownedInfo;
		VkSamplerCreateInfo* pInfo;

	};

}