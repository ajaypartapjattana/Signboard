#pragma once

#include "Signboard/RHI/rhi.h"

namespace rndr {

	class presentationStage {
	private:
		contextual_registry<rhi::swapchain> swapchain;

		uint32_t presentationFamily = UINT32_MAX;

		struct {
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
			VkSurfaceKHR surface = VK_NULL_HANDLE;
			VkSurfaceFormatKHR surfaceFormat = { VK_FORMAT_UNDEFINED, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
			VkPresentModeKHR surfacePresentMode = VK_PRESENT_MODE_FIFO_KHR;
		} compatible;

		struct {
			VkExtent2D extent{};
			uint32_t imageCount{};
		} swapchainState;

	public:
		presentationStage() = default;
		presentationStage(const presentationStage&) = delete;
		presentationStage(presentationStage&& other) noexcept
			:
			swapchain(std::move(other.swapchain)),
			compatible(other.compatible),
			swapchainState(other.swapchainState)
		{

		}
		
		presentationStage& operator=(const presentationStage&) = delete;
		presentationStage& operator=(presentationStage&& other) noexcept {
			if (this == &other)
				return *this;

			swapchain = std::move(other.swapchain);
			compatible = other.compatible;
			swapchainState = other.swapchainState;

			return *this;
		}

		~presentationStage() = default;

		VkResult createPresentationConfiguration(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* presentationFamilyIndex, uint32_t selectedGraphicsFamilyIndex = UINT32_MAX) noexcept;
		VkResult createSwapchain(VkDevice device, uint32_t graphicsFamilyIndex) noexcept;


	};

}