#pragma once

#include "Signboard/RHI/rhi.h"

namespace rndr {

	class presentationStage {
	private:
		VkDevice r_device = VK_NULL_HANDLE;
		VkPhysicalDevice r_physicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR r_surface = VK_NULL_HANDLE;

		struct {
			VkRenderPass compositionPass;
		} passes;

		struct {
			uint32_t presentFamily = 0;
			VkQueue presentQueue = VK_NULL_HANDLE;

			VkSwapchainKHR swapchain = VK_NULL_HANDLE;
			VkExtent2D extent{};
			uint32_t imageCount = 0;

			std::vector<VkImageView> swapchainImageViews;
			std::vector<VkFramebuffer> framebuffers;
		
		} presentation;

		struct {
			VkSurfaceFormatKHR surfaceFormat{};
			VkPresentModeKHR surfacePresentMode{};
		
		} configuration;

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

		struct RenderJob {
			VkFence inFlight;

			VkSemaphore imageAvailable;
			VkSemaphore renderComplete;

			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
			
			VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		};

		std::vector<RenderJob> renderJobs;
		size_t jobHint = 0;

		std::vector<VkFence> presentJobs;

		VkResult configurePasses(VkDevice device) noexcept;
		VkResult configureBindings(VkDevice device) noexcept;
		VkResult configureRendering(VkDevice device) noexcept;
		VkResult configureExecution(VkDevice device) noexcept;

	public:
		presentationStage() = default;
		presentationStage(const presentationStage&) = delete;
		presentationStage(presentationStage&& other) = delete;
		
		presentationStage& operator=(const presentationStage&) = delete;
		presentationStage& operator=(presentationStage&& other) = delete;

		~presentationStage() noexcept;

		VkResult root(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) noexcept;

		VkResult configurePresentation() noexcept;
		VkResult stagePresentation(VkImageView view);

	};

}