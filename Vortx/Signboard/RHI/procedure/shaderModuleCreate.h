#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "Signboard/Core/core.h"

namespace rhi {

	class creDevice;

	class pmvShader;

	class pcdShaderModuleCreate {
	public:
		pcdShaderModuleCreate(const creDevice& device, VkShaderModuleCreateInfo* pCreateInfo = nullptr) noexcept;

		void target_source(sgb::span<const uint32_t> bin) noexcept;
		VkResult publish(pmvShader& shader, VkShaderStageFlagBits stage) const noexcept;

		void preset(VkShaderModuleCreateInfo* pCreateInfo) noexcept;
		void reset() noexcept;

	private:
		VkShaderModuleCreateInfo* fetch_basic(VkShaderModuleCreateInfo* pCreateInfo) noexcept;

	private:
		VkDevice r_device;

		std::unique_ptr<VkShaderModuleCreateInfo> m_ownedInfo;
		VkShaderModuleCreateInfo* pInfo;

	};

}