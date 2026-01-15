#include "VulkanRHI.h"

VulkanRHI::VulkanRHI(GLFWwindow* window)
	: vulkanContext(window),
	  device(vulkanContext.getInstance(), window),
	
	  swapchain(device, 1200, 1200),

	  graphicsCommandPool(device),
	  descriptorPool(createDescriptorPool())
{
	
}

VulkanRHI::~VulkanRHI() {
	device.waitIdle();
}

VulkanDescriptorPool VulkanRHI::createDescriptorPool() {
	DescriptorPoolSizeDesc UBO_pool{};
	UBO_pool.type = DescriptorType::UniformBuffer;
	UBO_pool.count = 512;

	DescriptorPoolSizeDesc Texture_pool{};
	Texture_pool.type = DescriptorType::SampledImage;
	Texture_pool.count = 512;

	DescriptorPoolSizeDesc Sampler_pool{};
	Sampler_pool.type = DescriptorType::TextureSampler;
	Sampler_pool.count = 32;

	DescriptorPoolDesc desc;
	desc.poolSizes = { UBO_pool, Texture_pool, Sampler_pool };
	desc.maxSets = 128;

	return VulkanDescriptorPool(device, desc);
}

RHIView VulkanRHI::getRHIView() {
	return RHIView{
		device,
		swapchain,
		graphicsCommandPool,
		descriptorPool
	};
}