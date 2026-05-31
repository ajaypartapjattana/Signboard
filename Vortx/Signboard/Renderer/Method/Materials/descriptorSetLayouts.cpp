#include "descriptorSetLayouts.h"

#include <map>

namespace rndr {

	descriptorSetLayouts::descriptorSetLayouts(const rhi::device& device, sgb::vltWrite<rhi::pmvDescriptorSetLayout> descriptorSetLayoutWrite) noexcept
		:
		_wrt(descriptorSetLayoutWrite),
		m_create(device)
	{

	}

	std::vector<uint32_t> descriptorSetLayouts::create(sgb::span<const shaderBinary*> shaders) {
		BindingMap merged;

		size_t _sSz = shaders.size();

		for (size_t i = 0; i < _sSz; ++i) {
			if (!shaders[i])
				continue;

			ShaderReflect reflect{ shaders[i]->data };
			
			BindingMap local;

			try {
				local = reflect.reflectDescriptorBindings((VkShaderStageFlags)shaders[i]->stage);
			}
			catch (const std::exception& e) {
				LOG_ERROR("FAILURE: shader_reflection! -> %s", e.what());
				continue;
			}

			for (auto& [key, binding] : local) {
				auto [it, inserted] = merged.try_emplace(key, std::move(binding));
				if (!inserted) {
					it->second.stageFlags |= binding.stageFlags;
				}
			}
		}

		std::map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> grouped;

		for (const auto& [key, binding] : merged) {
			grouped[key.set].push_back(binding);
		}

		std::vector<uint32_t> handles;
		handles.reserve(grouped.size());

		for (auto& [setIndex, bindings] : grouped) {
			m_create.target_bindings(bindings);

			auto builder = [&](rhi::pmvDescriptorSetLayout* dl) {
				m_create.publish(*dl);
			};

			handles.push_back(_wrt.construct(builder));
		}

		return handles;
	}

	uint32_t descriptorSetLayouts::create(sgb::span<const VkDescriptorSetLayoutBinding> bindings) {
		m_create.target_bindings(bindings);

		auto builder = [&](rhi::pmvDescriptorSetLayout* dl) {
			m_create.publish(*dl);
		};

		return _wrt.construct(builder);
	}

}