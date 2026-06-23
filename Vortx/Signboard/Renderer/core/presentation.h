#pragma once

#include "Signboard/RHI/rhi.h"

namespace rndr {

	struct PresentationStageQueueInfo {
		uint32_t graphicsFamilyIndex;
		VkQueue graphicsQueue;
		uint32_t presentFamilyIndex;
		VkQueue presentQueue;
	};

	class presentationStage {
	private:
		VkDevice r_device = VK_NULL_HANDLE;
		VkPhysicalDevice r_physicalDevice = VK_NULL_HANDLE;

		struct {
			VkSurfaceFormatKHR surfaceFormat{};

		} configuration;

		struct {
			VkRenderPass compositionPass = VK_NULL_HANDLE;
		} passes;

		struct {
			VkSampler sampler = VK_NULL_HANDLE;
			VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

			VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
			std::vector<VkDescriptorSet> descriptorSets;
			
		} bindings;

		struct {
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			VkPipeline pipeline = VK_NULL_HANDLE;
		} rendering;

		struct {
			uint32_t graphicsFamily = 0;
			VkQueue graphicsQueue = VK_NULL_HANDLE;
			uint32_t presentFamily = 0;
			VkQueue presentQueue = VK_NULL_HANDLE;

			VkCommandPool commandPool = VK_NULL_HANDLE;
			std::vector<VkCommandBuffer> commandBuffers;
		} execution;

		struct PresentationTarget {
			VkSurfaceKHR surface = VK_NULL_HANDLE;
			VkPresentModeKHR presentMode{};

			VkSwapchainKHR swapchain = VK_NULL_HANDLE;
			std::vector<VkImageView> imageViews;
			std::vector<VkFramebuffer> framebuffers;
			std::vector<VkFence> imageFence;

			VkExtent2D extent{};
			uint32_t imageCount = 0;
		};

		std::vector<PresentationTarget> presentationTargets;

		struct presentJob {
			VkFence inFlight;

			VkSemaphore imageAvailable;
			VkSemaphore renderComplete;

			VkDescriptorSet descriptorSet;
			
			VkCommandBuffer commandBuffer;
		};

		std::vector<presentJob> jobs;
		size_t oldJobHint = 0;

		static constexpr size_t MAX_CONCURRENT_PRESENTATION_JOB_COUNT = 3;

	public:
		presentationStage() = default;
		presentationStage(const presentationStage&) = delete;
		presentationStage(presentationStage&& other) = delete;
		
		presentationStage& operator=(const presentationStage&) = delete;
		presentationStage& operator=(presentationStage&& other) = delete;

		~presentationStage() noexcept = default;

		VkResult root(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR primarySurface, const PresentationStageQueueInfo* pQueueInfo) noexcept;

		VkResult targetSurface(VkSurfaceKHR surface, size_t* index) noexcept;
		void releaseSurface(size_t index, VkSurfaceKHR* pSurface) noexcept;

		VkResult configurePresentation(size_t targetIndex, uint32_t minImageCount) noexcept;
		
		VkResult presentationImagePool(const VkImageView* pImageViews, uint32_t count);
		VkResult stagePresentation(uint32_t imageIndex, uint32_t targetIndex, VkSemaphore waitSemaphore);

		void reset() noexcept;

	};

}