#include "Signboard/Renderer/Internal/rndr_pAccess.h"

#include "Signboard/Assets/io/io.h"

namespace rndr {

	creMethods::creMethods(const context& context, const presentation& presentation, const resources& resources) noexcept
		:
		r_swapchain(_pAccess::swapchain(presentation)),
		r_swapchainImageHandles(_pAccess::imageHandles(presentation)),

		m_passCreate(_pAccess::device(context), m_renderPasses),
		m_framebufferCreate(_pAccess::device(context), m_framebuffers, _pAccess::view(resources).images),

		m_descriptorLayoutCreate(_pAccess::device(context), m_descriptorLayouts),
		m_pipelineLayoutcreate(_pAccess::device(context), m_pipelineLayouts),
		m_pipelineCreate(_pAccess::device(context), m_pipelines)
	{

	}

	uint32_t creMethods::loadShader(const char* path, VkShaderStageFlagBits stage) {
		io::loader::file_loader loader;

		sgb::vltWrite<shaderBinary> _wrt{ m_shaderBins };

		auto _ctor = [&](shaderBinary* bin) {
			bin->stage = stage;

			try {
				bin->data = loader.load_SPIRV(path);
			}
			catch (const std::exception& e) {
				LOG_ERROR("FAILURE: shader_load -> %s", path, e.what());
				throw;
			}
		};

		return _wrt.construct(_ctor);
	}

	void creMethods::create_primaryTarget(const PipelineStageShaders& shaderSet) {
		uint32_t renderPassHandle = m_passCreate.createSwapchainRenderPass(r_swapchain);

		std::vector<uint32_t> imageHandles;
		imageHandles.resize(1);

		size_t _siSz = r_swapchainImageHandles.size();
		std::vector<uint32_t> framebufferHandles(_siSz);
		for (size_t i = 0; i < _siSz; ++i) {
			imageHandles[0] = r_swapchainImageHandles[i];
			framebufferHandles[i] = m_framebufferCreate.createFramebuffers(*m_renderPasses[renderPassHandle], imageHandles);
		}

		// --- --- ---

		std::vector<const shaderBinary*> pipeShaders = { m_shaderBins[shaderSet.vertex], m_shaderBins[shaderSet.fragment] };
		
		std::vector<uint32_t> descriptorHandles = m_descriptorLayoutCreate.create(pipeShaders);

		std::vector<const rhi::pmvDescriptorSetLayout*> descriptorLayouts;
		descriptorLayouts.reserve(descriptorHandles.size());
		for (const uint32_t handle : descriptorHandles) {
			descriptorLayouts.emplace_back(m_descriptorLayouts[handle]);
		}

		std::vector<VkPushConstantRange> pushConstantRanges = {};

		uint32_t pipelineLayoutHandle = m_pipelineLayoutcreate.create(descriptorLayouts, pushConstantRanges);

		uint32_t pipelineHandle = m_pipelineCreate.createPipeline(*m_renderPasses[renderPassHandle], 0, *m_pipelineLayouts[pipelineLayoutHandle], pipeShaders);

		auto _ctor = [&](renderTarget* rtg) {
			renderTarget& target = *rtg;

			target.renderPassIndex = renderPassHandle;
			target.framebufferIndices = std::move(framebufferHandles);
			target.pipelineIndices.push_back(pipelineHandle);
			};

		sgb::vltWrite<renderTarget> _wrt{ m_renderTargets };
		primaryTarget_idx = _wrt.construct(_ctor);
	}

	void creMethods::validate_primaryTarget() {
		sgb::vltWrite<renderTarget> _wrt{ m_renderTargets };
		renderTarget& primary = *_wrt.get(primaryTarget_idx);

		std::vector<uint32_t> imageHandles;
		imageHandles.resize(1);

		size_t _iSz = r_swapchainImageHandles.size();
		primary.framebufferIndices.resize(_iSz);
		for (size_t i = 0; i < _iSz; ++i) {
			imageHandles[0] = r_swapchainImageHandles[i];
			primary.framebufferIndices[i] = m_framebufferCreate.createFramebuffers(*m_renderPasses[primary.renderPassIndex], imageHandles);
		}

	}

}


/*

	*/

	/*matInfo.shaders[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		matInfo.shaders[0].data = _loader.load_SPIRV("shaders/baseDS.vert.spv");

		matInfo.shaders[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		matInfo.shaders[1].data = _loader.load_SPIRV("shaders/base.frag.spv");*/