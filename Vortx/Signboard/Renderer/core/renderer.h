#pragma once

#include "Signboard/Renderer/Internal/rndr_pInterface.h"
#include "Signboard/Renderer/primitive/rndr_primitive.h"

constexpr uint32_t DEFAULT_BUFFERED_FRAME_COUNT = 2;

namespace rndr {

	class context;
	class resources;
	class presentation;
	class renderConfig;

	class Renderer {
	public:
		Renderer(const context& context, const presentation& presentation, const renderConfig& methods) noexcept;

		~Renderer() noexcept;

		void bindResources(const resources& resources);
		void bindMehtods(const renderConfig& methods);

		void syncPresentation();
		void set_bufferedFrameCount(uint32_t count);
		
		void render();
		void queueUpload(const Model& model, uint32_t allocatedMesh);

	private:
		bool prepareFrame(uint32_t frameIndex) noexcept;
		bool defferUploads();
		
		void advance_frame() noexcept;

	private:
		const context* r_context;
		const presentation* r_presentation;
		const rhi::swapchain& r_swapchain;

		resourceView r_resources;
		methodView r_methods;
		sgb::vltQuery<renderTarget> targets;

		rhi::stdCommandPools m_commandPools;

		std::vector<job> frames;
		uint32_t availableFrameCount;
		uint32_t activeFrameIndex = 0;

		std::vector<uint32_t> imagesInFlight;

		rhi::pcdWatchdog m_watchdog;

		rhi::pcdCommandBufferRecord m_CommandRecord{};
		rhi::pcdRenderPassRecord m_passRecord{};

		Scheduler m_interface;
		transferControl m_transfer;

	};

}