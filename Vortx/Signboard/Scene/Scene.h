#pragma once

#include "Signboard/RHI/vulkan/VulkanDescriptorLayout.h"
#include "Signboard/RHI/vulkan/VulkanDescriptorSet.h"

#include "Resources/SceneResources.h"

class VulkanDevice;
class VulkanDescriptorPool;

struct SceneView {

};

class Scene {
public:
	Scene(VulkanDevice&);

	SceneView getSceneView();

private:
	VulkanDescriptorSetLayout createViewStateLayout();
	VulkanDescriptorSetLayout createObjectStateLayout();
	VulkanDescriptorSetLayout createMaterialStateLayout();

private:
	VulkanDevice& device;
	VulkanDescriptorPool& descriptorPool;

	VulkanDescriptorSetLayout viewStateLayout;
	VulkanDescriptorSetLayout objectStateLayout;
	VulkanDescriptorSetLayout materialStateLayout;

	VulkanDescriptorSet viewStateSet;
	VulkanDescriptorSet objectStateSet;
	VulkanDescriptorSet materialStateSet;

	ViewStateSystem cameras;

	ObjectSystem objects;
	MaterialSystem materials;

};