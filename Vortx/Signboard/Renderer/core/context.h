#pragma once

#include "Signboard/RHI/core/rhi_core.h"

namespace rndr {

	struct _pAccess;

	enum QueueFamilyType : uint32_t {
		FAMILY_INDEX_GRAPHICS,
		FAMILY_INDEX_COMPUTE,
		FAMILY_INDEX_TRANSFER,

		FAMILY_INDEX_MAX_ENUM
	};

	class context {
	private:
		friend struct _pAccess;

		rhi::instance instance;
		std::vector<rhi::physicalDevice> physicalDevices;

		rhi::surface surface;

		uint32_t selectedPhysicalDeviceIndex;

		rhi::device device;
		std::array<uint32_t, FAMILY_INDEX_MAX_ENUM> assignedQueueFamilies{};
		uint32_t assignedPresentFamily;

		rhi::allocator allocator;

	public:
		context();

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		uint32_t familyIndex(QueueFamilyType type) const noexcept {
			return assignedQueueFamilies[type];
		}

		uint32_t presentationFamily() const noexcept {
			return assignedPresentFamily;
		}

		void createDevice(GLFWwindow* window, uint32_t physicalDeviceIndex);

	};

}
