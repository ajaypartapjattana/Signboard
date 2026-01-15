#include "Scene.h"

Scene::Scene(VulkanDevice& device) 
	:	device(device),
		
		viewStateLayout(createViewStateLayout()),
		objectStateLayout(createObjectStateLayout()), 
		materialStateLayout(createMaterialStateLayout()),

		viewStateSet	(descriptorPool.allocate(viewStateLayout		, nullptr)),
		objectStateSet	(descriptorPool.allocate(objectStateLayout		, nullptr)),
		materialStateSet(descriptorPool.allocate(materialStateLayout	, nullptr)),

		cameras			(device, viewStateSet		, DESCRIPTOR_SCHEMA::VIEW_STATE::VIEW_STATE_BINDING																			),
		objects			(device, objectStateSet		, DESCRIPTOR_SCHEMA::OBJECT_STATE::OBJECT_BUFFER_BINDING		, DESCRIPTOR_SCHEMA::OBJECT_STATE::MAX_OBJECT_COUNT			),
		materials		(device, materialStateSet	, DESCRIPTOR_SCHEMA::MATERIAL_VARIABLES::MATERIAL_BUFFER_BINDING, DESCRIPTOR_SCHEMA::MATERIAL_VARIABLES::MAX_MATERIAL_COUNT	)
{

}

#include "Signboard/RHI/vulkan/VulkanDescriptorLayout.h"
#include "Signboard/Renderer/DescriptorSchema.h"

VulkanDescriptorSetLayout Scene::createViewStateLayout() {
	DescriptorBindingDesc viewState{};
	viewState.binding = DESCRIPTOR_SCHEMA::VIEW_STATE::VIEW_STATE_BINDING;
	viewState.type = DescriptorType::UniformBuffer;
	viewState.stages.set(ShaderStageBit::VertexBit, ShaderStageBit::FragmentBit);
	viewState.count = DESCRIPTOR_SCHEMA::VIEW_STATE::VIEW_STATE_COUNT;

	DescriptorSetLayoutDesc desc{};
	desc.bindings = { viewState };

	return VulkanDescriptorSetLayout(device, desc);
}

VulkanDescriptorSetLayout Scene::createObjectStateLayout() {
	DescriptorBindingDesc objectState{};
	objectState.binding = DESCRIPTOR_SCHEMA::OBJECT_STATE::OBJECT_BUFFER_BINDING;
	objectState.type = DescriptorType::StorageBuffer;
	objectState.stages.set(ShaderStageBit::VertexBit);
	objectState.count = DESCRIPTOR_SCHEMA::OBJECT_STATE::MAX_OBJECT_COUNT;

	DescriptorSetLayoutDesc desc{};
	desc.bindings = { objectState };

	return VulkanDescriptorSetLayout(device, desc);
}

VulkanDescriptorSetLayout Scene::createMaterialStateLayout() {
	DescriptorBindingDesc MaterialParams{};
	MaterialParams.binding = DESCRIPTOR_SCHEMA::MATERIAL_VARIABLES::MATERIAL_BUFFER_BINDING;
	MaterialParams.type = DescriptorType::StorageBuffer;
	MaterialParams.stages.set(ShaderStageBit::VertexBit, ShaderStageBit::FragmentBit);
	MaterialParams.count = DESCRIPTOR_SCHEMA::MATERIAL_VARIABLES::MAX_MATERIAL_COUNT;

	DescriptorSetLayoutDesc desc{};
	desc.bindings = { MaterialParams };

	return VulkanDescriptorSetLayout(device, desc);
}