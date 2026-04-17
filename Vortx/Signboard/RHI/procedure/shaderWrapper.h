#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	class creDevice;

	class pmvShader;

	class pcdShaderModuleCreate {
	public:
		pcdShaderModuleCreate(const creDevice& device, VkShaderModuleCreateInfo* pCreateInfo = nullptr) noexcept;

		pcdShaderModuleCreate& target_source(const std::vector<uint32_t>& bin);
		VkResult publish(pmvShader& shader) const noexcept;

		void preset(VkShaderModuleCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkShaderModuleCreateInfo fetch_basic(VkShaderModuleCreateInfo* pCreateInfo) const noexcept;

	private:
		VkDevice r_device;

		const std::vector<uint32_t>* _pBin = nullptr;
		VkShaderModuleCreateInfo _info;

	};

}