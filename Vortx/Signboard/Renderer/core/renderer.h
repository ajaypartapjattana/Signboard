#pragma once

#include "Signboard/Renderer/Internal/rndr_pInterface.h"
#include "Signboard/Renderer/primitive/rndr_primitive.h"

constexpr uint32_t DEFAULT_BUFFERED_FRAME_COUNT = 2;

namespace rndr {

	class creContext;
	class creResources;
	class crePresentation;
	class creMethods;

	class Renderer {
	public:
		Renderer(const creContext& context, const crePresentation& presentation, const creMethods& methods) noexcept;

		~Renderer() noexcept;

		void bindResources(const creResources& resources);
		void bindMehtods(const creMethods& methods);

		void syncPresentation();
		void set_bufferedFrameCount(uint32_t count);
		
		void render();
		void advance_frame() noexcept;

		void queueUpload(const Model& model, uint32_t allocatedMesh);

	private:
		bool prepareFrame(const frame& frame) noexcept;
		bool defferUploads();

	private:
		const creContext* r_context;
		const crePresentation* r_presentation;
		const rhi::creSwapchain& r_swapchain;

		resourceView r_resources;
		methodView r_methods;
		sgb::vltQuery<renderTarget> targets;

		rhi::stdCommandPools m_commandPools;

		std::vector<frame> frames;
		std::vector<rhi::pmvSemaphore> imageRenderFinished;
		std::vector<uint32_t> imagesInFlight;

		uint32_t availableFrameCount = 2;
		uint32_t activeFrameIndex = 0;
		uint32_t imageIndex;
		uint32_t acquiredImage = 0;

		rhi::pcdWatchdog m_watchdog;

		rhi::pcdCommandBufferRecord m_CommandRecord{};
		rhi::pcdRenderPassRecord m_passRecord{};

		rndr_interface m_interface;
		transferControl m_transfer;

	};

}