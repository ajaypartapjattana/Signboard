#pragma once

#include "Signboard/RHI/core/rhi_core.h"

namespace rndr {

	enum QueueFamilyType : uint32_t {
		FAMILY_INDEX_GRAPHICS,
		FAMILY_INDEX_COMPUTE,
		FAMILY_INDEX_TRANSFER,

		FAMILY_INDEX_MAX_ENUM
	};

	struct DeviceInfo {
		VkSurfaceKHR candidateSurface;
		VkPhysicalDevice nativeDevice;
		VkDevice logicalDevice;
		std::array<uint32_t, FAMILY_INDEX_MAX_ENUM> assignedQueueFamilies{};
		uint32_t assignedPresentFamily;

		VmaAllocator resourceAllocator;
	};

	class context {
	private:
		rhi::instance instance;
		std::vector<rhi::physicalDevice> physicalDevices;
		rhi::surface surface;
		rhi::device device;
		rhi::allocator allocator;

		void createInstance();

	public:
		context() {
			createInstance();
		}
		context() noexcept {
			createInstance();
		}
		context(const context&) = delete;

		context& operator=(const context&) = delete;

		~context() noexcept {

		}

		void enumeratePhysicalDevices(uint32_t* pCount, const char** pNames) const noexcept {
			if (!pCount)
				return;

			*pCount = static_cast<uint32_t>(physicalDevices.size());

			if (!pNames)
				return;

			for (uint32_t i = 0; i < *pCount; ++i) {
				pNames[i] = physicalDevices[i].deviceName();
			}
		}

		void createDevice(GLFWwindow* window, uint32_t physicalDeviceIndex, DeviceInfo* pInfo);

	};

}
