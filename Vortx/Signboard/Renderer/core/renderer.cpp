#include "Signboard/Renderer/Internal/rndr_pAccess.h"

#include <algorithm>

namespace rndr {

	Renderer::Renderer(const creContext& context, const crePresentation& presentation, const creMethods& methods) noexcept
		:
		r_context(&context),
		r_presentation(&presentation),
		r_swapchain(_pAccess::swapchain(presentation)),

		m_watchdog(_pAccess::device(context)),
		m_interface(context, presentation),
		m_transfer(_pAccess::allocator(context))
	{
		rhi::pcdCommandPoolCreate pcd{ _pAccess::device(context) };
		pcd.create(m_commandPools);
	}

	Renderer::~Renderer() noexcept {
		m_watchdog.wait_device();
	}

	void Renderer::bindResources(const creResources& resources) {
		r_resources = _pAccess::view(resources);
		m_transfer.bindResources(r_resources);
	}

	void Renderer::bindMehtods(const creMethods& methods) {
		r_methods = _pAccess::view(methods);
		targets = _pAccess::targets(methods);
	}

	void Renderer::set_bufferedFrameCount(uint32_t count) {
		if (count == frames.size())
			return;

		if (count > imagesInFlight.size())
			throw std::logic_error("FALLBACK: buffered_frame_count > swapcahin_image_count is SUB-OPTIMAL!");

		const uint32_t _fSz = static_cast<uint32_t>(frames.size());
		const uint32_t _shrd = std::min(count, _fSz);

		frames.reserve(count);
		imageRenderFinished.reserve(count);

		const rhi::creDevice& r_device = _pAccess::device(*r_context);
		rhi::pcdCommandBufferAllocate pcd{ r_device };
		for (uint32_t i = _shrd; i < count; ++i) {
			frames.emplace_back(r_device);

			pcd.target_commandPool(m_commandPools.graphicsPool);
			pcd.publish({ &frames[i].CMDGraphics, 1 });

			pcd.target_commandPool(m_commandPools.transferPool);
			pcd.publish({ &frames[i].CMDTransfer, 1 });

			imageRenderFinished.emplace_back(r_device);
		}

		frames.resize(count);
		imageRenderFinished.resize(count);

		availableFrameCount = count;
	}

	void Renderer::syncPresentation() {
		m_interface.validate_swapchainDependancy();
		imagesInFlight.resize(r_presentation->availableImageCount());
	}

	void Renderer::render() {
		if (!prepareFrame(frames[activeFrameIndex])) {
			throw std::runtime_error("INCOMPATIBILITY: swapchain_out_of_date!");
		}

		bool waitUploads = defferUploads();

		{
			m_CommandRecord.begin();
			for (const renderTarget& target : targets) {
				m_passRecord.target_commandBuffer(frames[activeFrameIndex].CMDGraphics);
				m_passRecord.begin(*r_methods.renderPasses[target.renderPassIndex], *r_methods.framebuffers[target.framebufferIndices[imageIndex]]);

				for (const uint32_t& pipeline : target.pipelineIndices) {
					m_passRecord.bind_pipeline(*r_methods.pipelines[pipeline]);

					for (const Mesh& m : r_resources.meshes) {
						std::vector<const rhi::pmvBuffer*> vertexBuffers = { r_resources.buffers[m.vertexBuffer] };
						std::vector<VkDeviceSize> offsets = { 0 };

						m_passRecord.bind_vertexBuffer(vertexBuffers, offsets);
						m_passRecord.bind_indexBuffer(*r_resources.buffers[m.indexBuffer]);

						m_passRecord.draw(m.indexCount);
					}

				}

				m_passRecord.end();
			}
			
			m_CommandRecord.end();
		}

		m_watchdog.reset_fence(frames[activeFrameIndex].frameInFlight);
		m_interface.pushRenderJob(frames[activeFrameIndex]);
		m_interface.pushPresentJob();

		m_interface.advance_frame();
	}

	void Renderer::advance_frame() noexcept {
		activeFrameIndex = (activeFrameIndex + 1) % availableFrameCount;
	}

	bool Renderer::prepareFrame(const frame& frame) noexcept{
		m_watchdog.watch_fence(frame.frameInFlight);

		VkResult result = r_swapchain.acquireImage(frame.imageAvailable, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
			return false;

		int32_t frameIndex = imagesInFlight[imageIndex];
		if (frameIndex != -1) {
			m_watchdog.watch_fence(frames[frameIndex].frameInFlight);
		}

		imagesInFlight[imageIndex] = activeFrameIndex;

		return true;
	}

	bool Renderer::defferUploads() {
		VkResult result = m_transfer.recordUploads(frames[activeFrameIndex].CMDTransfer);

		if (result == VK_INCOMPLETE)
			return false;

		m_interface.pushUploadJob(frames[activeFrameIndex]);
		m_transfer.reset();

		return true;
	}

	void Renderer::queueUpload(const Model& model, uint32_t allocatedMesh) {
		const Mesh& mesh = *r_resources.meshes.get(allocatedMesh);

		m_transfer.stageUpload(UploadSpan(model.vertices), { mesh.vertexBuffer, 0 });
		m_transfer.stageUpload(UploadSpan(model.indices), { mesh.indexBuffer, 0 });
	}

}
