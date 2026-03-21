#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::core {

	struct instance_vkAccess;

	class instance {
	public:
		struct createInfo {
			std::vector<const char*> extensions;
			std::vector<const char*> layers;
			bool enable_validation = false;
		};

		instance(const createInfo& createInfo) noexcept;

		instance(const instance&) = delete;
		instance& operator=(const instance&) = delete;

		instance(instance&&) noexcept;
		instance& operator=(instance&&) noexcept;

		~instance() noexcept;

		const VkInstance native_instance() const noexcept;

	private:
		void build(const createInfo& createInfo);

		bool validateExtension(const char* extName);
		bool validateLayer(const char* layerName);

	private:
		friend struct instance_vkAccess;

		VkInstance m_instance = VK_NULL_HANDLE;

	};

}