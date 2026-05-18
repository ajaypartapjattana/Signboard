#pragma once

#include <vulkan/vulkan.h>

namespace rhi {

	struct _pAccess;

	class pcdCommandPoolCreate;

	class pmvCommandPool {
	public:
		pmvCommandPool() noexcept;

		pmvCommandPool(const pmvCommandPool&) noexcept;
		pmvCommandPool& operator=(const pmvCommandPool&) noexcept;

		pmvCommandPool(pmvCommandPool&&) noexcept;
		pmvCommandPool& operator=(pmvCommandPool&&) noexcept;

		~pmvCommandPool() noexcept;

		void reset() noexcept;

	private:
		friend class pcdCommandPoolCreate;
		friend struct _pAccess;

		VkCommandPool m_commandPool;
		uint32_t family;

		VkDevice r_device;
	};

}