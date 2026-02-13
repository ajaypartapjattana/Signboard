#pragma once

#include "vulkan/VulkanContext.h"
#include "vulkan/VulkanDevice.h"

#include "vulkan/VulkanSwapchain.h"

#include "vulkan/VulkanCommandPool.h"
#include "vulkan/VulkanDescriptorPool.h"

struct RHIView {
	VulkanDevice& device;
	VulkanSwapchain& swapchain;
	VulkanCommandPool& commandPool;
	VulkanDescriptorPool& descriptorPool;
};

class GLFWwindow;

class VulkanRHI {
public:
	VulkanRHI(GLFWwindow* window);
	~VulkanRHI();

	RHIView getRHIView();

	VulkanDevice& getDevice() { return device; }
	VulkanDescriptorPool& getDescriptorPool() { return descriptorPool; }

private:
	VulkanDescriptorPool createDescriptorPool();

private:
	VulkanContext vulkanContext;
	VulkanDevice device;

	VulkanSwapchain swapchain;

	VulkanCommandPool graphicsCommandPool;
	VulkanDescriptorPool descriptorPool;
};