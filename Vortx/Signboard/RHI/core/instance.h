#pragma once

namespace rhi::procedure {
	class instance_builder;
}

#include <vulkan/vulkan.h>

namespace rhi::core {

	struct instance_vkAccess;

	class instance {
	public:
		instance(const instance&) = delete;
		instance& operator=(const instance&) = delete;

		instance(instance&&) noexcept;
		instance& operator=(instance&&) noexcept;

		~instance() noexcept;

		const VkInstance* native_instance() const noexcept;

	private:
		friend class rhi::procedure::instance_builder;
		friend struct instance_vkAccess;

		instance() = default;

		VkInstance m_instance = nullptr;
		VkAllocationCallbacks* m_allocator = nullptr;

	};

}