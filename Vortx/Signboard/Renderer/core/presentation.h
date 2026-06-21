#pragma once

#include "Signboard/RHI/rhi.h"

namespace rndr {

	struct PresentationStageCreateInfo {
		uint32_t presentFamilyIndex;
		VkQueue presentQueue;
		uint32_t graphicsFamilyIndex;
		VkQueue graphicsQueue;
	};

	class presentationStage {
	private:
		VkDevice r_device = VK_NULL_HANDLE;
		VkPhysicalDevice r_physicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR r_surface = VK_NULL_HANDLE;

		struct {
			VkRenderPass compositionPass;
		} passes;

		struct {
			VkSampler sampler;
			VkDescriptorSetLayout descriptorSetLayout;

			VkDescriptorPool descriptorPool;
			std::vector<VkDescriptorSet> descriptorSets;
			
		} bindings;

		struct {
			VkPipelineLayout pipelineLayout;
			VkPipeline pipeline;
		} rendering;

		struct {
			uint32_t graphicsFamily = 0;
			VkQueue graphicsQueue = VK_NULL_HANDLE;

			VkCommandPool commandPool;
			std::vector<VkCommandBuffer> commandBuffers;
		} execution;

		struct {
			uint32_t presentFamily = 0;
			VkQueue presentQueue = VK_NULL_HANDLE;

			VkSwapchainKHR swapchain = VK_NULL_HANDLE;
			std::vector<VkImageView> swapchainImageViews;
			std::vector<VkFramebuffer> framebuffers;

			VkExtent2D extent{};
			uint32_t imageCount = 0;

		} presentation;

		struct {
			VkSurfaceFormatKHR surfaceFormat{};
			VkPresentModeKHR surfacePresentMode{};

		} configuration;

		struct presentJob {
			VkFence inFlight;

			VkSemaphore imageAvailable;
			VkSemaphore renderComplete;

			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
			
			VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		};

		std::vector<presentJob> jobs;
		size_t jobHint = 0;

	public:
		presentationStage() = default;
		presentationStage(const presentationStage&) = delete;
		presentationStage(presentationStage&& other) = delete;
		
		presentationStage& operator=(const presentationStage&) = delete;
		presentationStage& operator=(presentationStage&& other) = delete;

		~presentationStage() noexcept;

		VkResult root(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const PresentationStageCreateInfo* pCreateInfo) noexcept;

		VkResult configurePresentation(uint32_t minImageCount) noexcept;

		VkResult presentationImagePool(const VkImageView* pImageViews, uint32_t count);
		VkResult stagePresentation(uint32_t imageIndex, VkSemaphore waitSemaphore);

	};

}