#pragma once

#include "resourceSystems/Resources.h"

#include "Signboard/RHI/vulkan/VulkanDescriptorPool.h"
#include "Signboard/RHI/vulkan/VulkanDescriptorLayout.h"
#include "Signboard/RHI/vulkan/VulkanDescriptorSet.h"

#include "Signboard/Renderer/DescriptorSchema.h"

struct ResourceView {

};

struct MeshDesc;
struct TextureDesc;
struct SamplerDesc;

class VulkanDevice;
class VulkanDescriptorPool;
class VulkanCommandBuffer;

class ResourceAPI {
public:
	ResourceAPI(VulkanDevice& device, VulkanDescriptorPool& descriptorPool);
	~ResourceAPI() = default;

	ResourceView getResourceView();
	
	MeshHandle createMesh(VulkanCommandBuffer& cmd, const MeshDesc& desc);
	TextureHandle createTexture(VulkanCommandBuffer& cmd, const TextureDesc& desc);
	SamplerHandle createSampler(const SamplerDesc& desc);
	PipelineHandle createPipeline();
	//MaterialHandle createMaterial(const MaterialDesc& desc);
	//ObjectHandle createObject(MeshHandle mesh, MaterialHandle material, const glm::mat4& transform);

	void destroyMesh(MeshHandle mesh);
	void destroyTexture(TextureHandle texture);
	void destroySampler(SamplerHandle sampler);
	void destroyPipeline(PipelineHandle pipeline);

	void flush();

private:
	VulkanDescriptorSetLayout createBindlessTextureLayout();

private:
	VulkanDevice& device;
	VulkanDescriptorPool& descriptorPool;

	VulkanDescriptorSetLayout bindlessTextureLayout;
	VulkanDescriptorSet bindlessTextureSet;

	PipelineSystem pipelineSystem;
	TextureSystem textureSystem;
	SamplerSystem samplerSystem;
	MeshSystem meshSystem;

};