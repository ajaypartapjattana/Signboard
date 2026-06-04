#pragma once

#include "Signboard/Renderer/core/context.hh"
#include "Presentation/presentation.hh"
#include "Signboard/Renderer/Method/method.hh"

constexpr uint32_t DEFAULT_BUFFERED_FRAME_COUNT = 2;

namespace rndr {

	class Renderer {
	private:
		context renderContext;
		
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		VkSwapchainKHR swapchain;
		

		presentation presentationState;
		
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

		void createDescriptorSetLayouts();
		void createPipelineLayouts();

		void createRenderMethods(VkFormat backbufferImageFormat);
		VkFramebuffer createFramebuffer(VkRenderPass renderPass, sgb::span<const VkImageView> attachments, VkExtent2D extent);

		void validate_primaryTarget();

	public:
		Renderer() = default;
		Renderer(GLFWwindow* window);
			

		~Renderer() noexcept;

		void syncPresentation();
		void set_bufferedFrameCount(uint32_t count);
		
		void render();
		void queueUpload(const Model& model, uint32_t allocatedMesh);

	private:
		bool prepareFrame(uint32_t frameIndex) noexcept;
		bool defferUploads();
		
		void advance_frame() noexcept;

	};

}