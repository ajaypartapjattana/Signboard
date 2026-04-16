#include "materials.h"

#include "Signboard/Assets/shaderReflect/shaderReflect.h"
#include <map>

materials::materials(const rhi::creDevice& device, const rhi::pmvSwapchain& swapchain, ctnr::vltView<rhi::pmvRenderPass> passAccess, ctnr::vltView<rhi::pmvVertexLayout> fieldsAccess)
	: 
	r_device(device), 
	r_swapchain(swapchain),

	a_renderPassView(std::move(passAccess)),
	a_vertexLayoutView(std::move(fieldsAccess))
{
	
}

uint32_t materials::createPipelineLayout(const std::vector<shaderBinary>& shaders) {
	std::vector<DescriptorBinding> allBindings;

	for (const shaderBinary& bin : shaders) {
		ShaderReflect reflect{ bin.data };

		std::vector<DescriptorBinding> bindings = reflect.reflectDescriptorBindings((VkShaderStageFlags)bin.stage);

		for (DescriptorBinding& b : bindings) {
			bool found = false;

			for (DescriptorBinding& existing : allBindings) {
				if (existing.set != b.set || existing.binding != b.binding) 
					continue;
				
				existing.stage |= b.stage;
				found = true;
				break;
			}

			if (!found)
				allBindings.push_back(b);
		}
	}

	std::map<uint32_t, std::vector<DescriptorBinding>> sets;
	for (DescriptorBinding& b : allBindings) {
		sets[b.set].push_back(b);
	}
	
	ctnr::vault_writeAccessor<rhi::pmvDescriptorLayout> _DLwrt{ m_descriptorLayouts };
	std::vector<uint32_t> descHandles;
	for (auto& [setIndex, bindings] : sets) {
		rhi::pcdDescriptorLayoutCreate DLCreate{ r_device };

		std::vector<rhi::pcdDescriptorLayoutCreate::binding> DLBindings;

		uint32_t _bSz = static_cast<uint32_t>(bindings.size());
		DLBindings.reserve(_bSz);
		for (DescriptorBinding& b : bindings) {
			DLBindings.push_back({ b.binding, b.type, b.count, b.stage });
		}

		DLCreate.push_bindings(DLBindings);

		auto builder = [&](rhi::pmvDescriptorLayout* dl) {
			DLCreate.create(*dl);
		};

		descHandles.push_back(_DLwrt.construct(builder));
	}

	rhi::pcdPipelineLayoutCreate PLCreate{ r_device };

	for (uint32_t h : descHandles) {
		const rhi::pmvDescriptorLayout& _dl = *_DLwrt.get(h);
		PLCreate.add_setLayout(_dl);
	}

	auto builder = [&](rhi::pmvPipelineLayout* pl) {
		PLCreate.build(*pl);
	};

	ctnr::vault_writeAccessor<rhi::pmvPipelineLayout> _wrt{ m_pipelineLayouts };
	return _wrt.construct(builder);
}

uint32_t materials::createPipeline(uint32_t renderPassIndex, uint32_t subpass, uint32_t pipelineLayoutIndex, const pipelineCreateInfo& info) {
	ctnr::vault_writeAccessor<rhi::pmvPipelineLayout> _PLView{ m_pipelineLayouts };
	const rhi::pmvPipelineLayout& pipeLayout = *_PLView.get(pipelineLayoutIndex);
	
	rhi::pcdPipelineBuilder prcdr{ r_device, r_swapchain, pipeLayout };
	prcdr.set_vertexLayout(a_vertexLayoutView.get(info.vertexLayoutIndex));

	uint32_t _shaderSz = static_cast<uint32_t>(info.shaders.size());
	std::vector<rhi::pmvShader> _pipeShaders(_shaderSz);

	rhi::pcdShaderWrapper wrapper{ r_device };
	
	for (uint32_t i = 0; i < _shaderSz; ++i) {
		const shaderBinary& bin = info.shaders[i];
		rhi::pmvShader& tgt = _pipeShaders[i];

		wrapper.setBinary(bin.data);
		wrapper.wrapShaderCode(tgt);

		prcdr.push_shader(bin.stage, tgt);
	}

	prcdr.set_targetPass(a_renderPassView.get(renderPassIndex));

	auto builder = [&](rhi::pmvPipeline* p) {
		prcdr.build_graphicsPipeline(subpass, *p);
	};
	
	ctnr::vault_writeAccessor<rhi::pmvPipeline> _wrt{ m_pipelines };
	return _wrt.construct(builder);
}

ctnr::vltView<rhi::pmvPipeline> materials::read_pipelines() const noexcept {
	return ctnr::vltView<rhi::pmvPipeline>{ m_pipelines };
}
