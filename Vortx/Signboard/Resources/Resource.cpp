#include "Resource.h"

ResourceAPI::ResourceAPI(VulkanDevice& device, VulkanDescriptorPool& descriptorPool)
	: device(device),
	  descriptorPool(descriptorPool),

	  bindlessTextureLayout(createBindlessTextureLayout()),

	  bindlessTextureSet(descriptorPool.allocate(bindlessTextureLayout, DESCRIPTOR_SCHEMA::BINDLESS_TEXTURES::VARIABLE_COUNT)),

	  textureSystem(device, bindlessTextureSet, DESCRIPTOR_SCHEMA::BINDLESS_TEXTURES::TEXTURE_BINDING, DESCRIPTOR_SCHEMA::BINDLESS_TEXTURES::MAX_TEXTURES),
	  samplerSystem(device, bindlessTextureSet, DESCRIPTOR_SCHEMA::BINDLESS_TEXTURES::SAMPLER_BINDING, DESCRIPTOR_SCHEMA::BINDLESS_TEXTURES::MAX_SAMPLERS),
	  pipelineSystem(device),
	  meshSystem(device)
{

}

VulkanDescriptorSetLayout ResourceAPI::createBindlessTextureLayout() {
	DescriptorBindingDesc textures{};
	textures.binding = DESCRIPTOR_SCHEMA::BINDLESS_TEXTURES::TEXTURE_BINDING;
	textures.type = DescriptorType::SampledImage;
	textures.stages.set(ShaderStageBit::FragmentBit);
	textures.count = DESCRIPTOR_SCHEMA::BINDLESS_TEXTURES::MAX_TEXTURES;
	textures.flags.set(DescriptorBindingBit::PatiallyBound, DescriptorBindingBit::UpdateAfterBind, DescriptorBindingBit::VariableCount);

	DescriptorBindingDesc samplers{};
	samplers.binding = DESCRIPTOR_SCHEMA::BINDLESS_TEXTURES::SAMPLER_BINDING;
	samplers.type = DescriptorType::TextureSampler;
	samplers.stages.set(ShaderStageBit::FragmentBit);
	samplers.count = DESCRIPTOR_SCHEMA::BINDLESS_TEXTURES::MAX_SAMPLERS;
	samplers.flags.set(DescriptorBindingBit::PatiallyBound, DescriptorBindingBit::UpdateAfterBind, DescriptorBindingBit::VariableCount);

	DescriptorSetLayoutDesc desc{};
	desc.bindings = { textures, samplers };

	return VulkanDescriptorSetLayout(device, desc);
}

MeshHandle ResourceAPI::createMesh(VulkanCommandBuffer& cmd, const MeshDesc& desc) {
	return meshSystem.createMesh(cmd, desc);
}

ResourceView ResourceAPI::getResourceView() {
	return ResourceView{ 
		pipelineSystem, 
		textureSystem, 
		samplerSystem, 
		meshSystem 
	};
}

