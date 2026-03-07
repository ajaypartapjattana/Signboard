#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::core {

	struct instance_vkAccess;
	
	struct instance_CI {
		std::vector<const char*> extensions;
		std::vector<const char*> layers;
		bool enable_validation = false;
	};

	class instance {
	public:
		instance(const instance_CI& createInfo) noexcept;

		instance(const instance&) = delete;
		instance& operator=(const instance&) = delete;

		instance(instance&&) noexcept;
		instance& operator=(instance&&) noexcept;

		~instance() noexcept;

		const VkInstance native_instance() const noexcept;

	private:
		void build(const instance_CI& createInfo);

		bool validateExtension(const char* extName);
		bool validateLayer(const char* layerName);

	private:
		friend struct instance_vkAccess;

		VkInstance m_instance = VK_NULL_HANDLE;

	};

}