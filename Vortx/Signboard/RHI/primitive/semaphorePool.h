#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace rhi {

	struct sempahore_traits {

		using createInfo = VkSemaphoreCreateInfo;

		VkResult create(VkDevice device, createInfo* pInfo, VkSemaphore* sempahore) noexcept {
			return vkCreateSemaphore(device, pInfo, nullptr, sempahore);
		}

		void destroy(VkDevice device, VkSemaphore semaphore) noexcept {
			vkDestroySemaphore(device, semaphore, nullptr);
		}

	};

	struct _pAccess;

	class creDevice;
	class pcdQueueSubmit;

	class pmvSemaphorePool {
	public:
		pmvSemaphorePool() noexcept = default;

		pmvSemaphorePool(const creDevice& device, uint32_t capacity, VkSemaphoreCreateInfo* pCreateInfo = nullptr) noexcept;
		pmvSemaphorePool(const creDevice& device, VkSemaphoreCreateInfo* pCreateInfo = nullptr) noexcept;

		pmvSemaphorePool(const pmvSemaphorePool&) = delete;
		pmvSemaphorePool& operator=(const pmvSemaphorePool&) = delete;

		pmvSemaphorePool(pmvSemaphorePool&&) noexcept;
		pmvSemaphorePool& operator=(pmvSemaphorePool&&) noexcept;

		void root(const creDevice& device);
		void resize(uint32_t size);

		VkSemaphore grant() noexcept;
		void reuse() noexcept;

		~pmvSemaphorePool() noexcept;

	private:
		VkSemaphoreCreateInfo* allot_basic(VkSemaphoreCreateInfo* pCreateInfo) noexcept;

	private:
		friend struct _pAccess;
		friend class pcdQueueSubmit;

		std::vector<VkSemaphore> m_semaphores;
		uint32_t m_size;

		uint32_t next = 0;

		std::unique_ptr<VkSemaphoreCreateInfo> m_ownedInfo;
		VkSemaphoreCreateInfo* pInfo;

		VkDevice r_device;

	};

}