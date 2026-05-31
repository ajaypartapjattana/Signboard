#pragma once

#include "Signboard/RHI/core/rhi_core.h"

namespace rndr {

	enum QueueFamilyType : uint32_t {
		FAMILY_INDEX_GRAPHICS,
		FAMILY_INDEX_COMPUTE,
		FAMILY_INDEX_TRANSFER,

		FAMILY_INDEX_MAX_ENUM
	};

	struct ContextPage {
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		std::array<uint32_t, FAMILY_INDEX_MAX_ENUM> assignedQueueFamilies{};
		uint32_t assignedPresentFamily;

		VmaAllocator allocator;
	};

	class context {
	private:
		rhi::instance instance;
		std::vector<rhi::physicalDevice> physicalDevices;

		rhi::surface surface;

		uint32_t selectedPhysicalDeviceIndex;

		rhi::device device;
		rhi::allocator allocator;

		ContextPage* pPage;

		void createInstance();

	public:
		context() {
			createInstance();
		}
		context(ContextPage* pContextPage) noexcept 
			:
			pPage(pContextPage)
		{
			createInstance();
		}
		context(const context&) = delete;

		context& operator=(const context&) = delete;

		~context() noexcept {

		}

		void maintain(ContextPage* pContextPage) noexcept {
			pPage = pContextPage;
		}

		void createDevice(GLFWwindow* window, uint32_t physicalDeviceIndex);

	};

}
