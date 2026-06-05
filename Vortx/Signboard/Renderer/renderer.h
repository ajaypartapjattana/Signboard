#pragma once

#include "Signboard/RHI/rhi.h"
#include <array>

constexpr uint32_t DEFAULT_BUFFERED_FRAME_COUNT = 2;

namespace rndr {

	enum QueueFamilyType : uint32_t {
		FAMILY_INDEX_GRAPHICS,
		FAMILY_INDEX_COMPUTE,
		FAMILY_INDEX_TRANSFER,

		FAMILY_INDEX_MAX_ENUM
	};

	class Renderer {
	private:
		rhi::instance instance;
		std::vector<rhi::physicalDevice> physicalDevices;
		size_t activePhysicalDeviceIndex;

		rhi::surface surface;
		rhi::device device;
		std::array<uint32_t, FAMILY_INDEX_MAX_ENUM> assignedQueueFamilies{};
		uint32_t assignedPresentFamily;

		rhi::allocator allocator;

		struct {
			VkSurfaceFormatKHR surfaceFormat;
			VkPresentModeKHR presentMode;
		} presentationInfo;
		
		rhi::swapchain swapchain;

		struct {
			uint32_t imageCount;
			VkExtent2D extent;
		} presentationState;
		
		contextual_pool<rhi::descriptorSetLayout> descriptorSetLayouts;
		struct {
			std::size_t gloabalSetLayout;
			std::size_t materialSetLayout;
			std::size_t objectSetLayout;
		} uniqueDescriptorSetLayoutIndices;

		contextual_pool<rhi::pipelineLayout> pipelineLayouts;
		struct {
			std::size_t PipelineLayout_2D;
			std::size_t PipelineLayout_3D;
		} uniquePipelineLayoutIndices;

		contextual_pool<rhi::renderPass> renderPasses;
		struct {
			std::size_t backbufferRenderPass;
		} uniqueRenderPassIndices;

		contextual_pool<rhi::graphicsPipeline> grpahicsPipelines;
		struct {
			std::size_t pipeline_3D;
		} uniquePipelineIndices;

		contextual_pool<rhi::framebuffer> framebuffers;
		std::vector<size_t> backbufferIndices;

		
		void createInstance();
		void createDevice(GLFWwindow* window, uint32_t physicalDeviceIndex);
		void selectPresentationConfiguration() noexcept;
		void createSwapchain();

		void createDescriptorSetLayouts();
		void createPipelineLayouts();

		void createRenderMethods(VkFormat backbufferImageFormat);
		VkFramebuffer createFramebuffer(VkRenderPass renderPass, sgb::span<const VkImageView> attachments, VkExtent2D extent);

		void validate_primaryTarget();

	public:
		Renderer() = default;
		Renderer(GLFWwindow* window);
			

		~Renderer() noexcept = default;

		void syncPresentation();
		void set_bufferedFrameCount(uint32_t count);
		
		void render();
	private:
		bool prepareFrame(uint32_t frameIndex) noexcept;
		bool defferUploads();
		
		void advance_frame() noexcept;

	};

}

/*rhi::stdCommandPools m_commandPools;

	std::vector<job> frames;
	uint32_t availableFrameCount;
	uint32_t activeFrameIndex = 0;

	std::vector<uint32_t> imagesInFlight;

	rhi::pcdWatchdog m_watchdog;

	rhi::pcdCommandBufferRecord m_CommandRecord{};
	rhi::pcdRenderPassRecord m_passRecord{};

	Scheduler m_interface;
	transferControl m_transfer;*/