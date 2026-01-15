#include "VulkanDevice.h"

#include "Common/VulkanCommon.h"
#include "TypeMap/VulkanDeviceTypeMap.h"

#include <GLFW/glfw3.h>
#include <vector>
#include <set>
#include <cstring>

VulkanDevice::VulkanDevice(VkInstance instance, GLFWwindow* window)
	: instance(instance)
{
	createSurface(window);
	pickPhysicalDevice();
	createLogicalDevice();
}

VulkanDevice::~VulkanDevice() {
	shutdown();
}

void VulkanDevice::shutdown() {
	if (device) {
		vkDestroyDevice(device, nullptr);
	}
}

void VulkanDevice::createSurface(GLFWwindow* window) {
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create surface");
	}
}

void VulkanDevice::pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("no vulkan GPUs found!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (VkPhysicalDevice device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			accquireQueueFamilies(device);
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("no suitable vulkan device found!");
	}
}

bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device) const {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.data());

	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			return true;
		}
	}
	return false;
}

#include <optional>

void VulkanDevice::accquireQueueFamilies(VkPhysicalDevice device) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.data());

	std::optional<uint32_t> graphics;
	std::optional<uint32_t> transfer;
	std::optional<uint32_t> present;

	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		if (!graphics && (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			graphics = i;
		}

		VkBool32  presentSupported = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupported);
		if (!present && presentSupported) {
			presentQueueFamily = i;
		}
	}

	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		const auto& flags = families[i].queueFlags;

		if ((flags & VK_QUEUE_TRANSFER_BIT) && !(flags & VK_QUEUE_GRAPHICS_BIT) && !(flags & VK_QUEUE_COMPUTE_BIT)) {
			transfer = i;
			break;
		}
	}

	if (!transfer)
		transfer = graphics;

	if(!graphics || !present || !transfer)
		throw std::runtime_error("failed to find graphics queue family!");

	graphicsQueueFamily = *graphics;
	transferQueueFamily = *transfer;
	presentQueueFamily = *present;
}

void VulkanDevice::createLogicalDevice() {
	std::set<uint32_t> uniqueFamilies = { graphicsQueueFamily, transferQueueFamily, presentQueueFamily };

	float priority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueInfos;

	for (uint32_t family : uniqueFamilies) {
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = family;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &priority;
		queueInfos.push_back(queueInfo);
	}

	VkPhysicalDeviceFeatures features{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
	createInfo.pEnabledFeatures = &features;

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(device, presentQueueFamily, 0, &presentQueue);

	if (transferQueueFamily == graphicsQueueFamily)
		transferQueue = graphicsQueue;
	else
		vkGetDeviceQueue(device, transferQueueFamily, 0, &transferQueue);
}

uint32_t VulkanDevice::findMemoryType(uint32_t typeFilter, MemoryPropertyFlags properties) const {
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

	VkMemoryPropertyFlags propertyFlag = toVkMemoryPropertyFlags(properties);
	for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (memProps.memoryTypes[i].propertyFlags & propertyFlag) == propertyFlag) {
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanDevice::waitIdle() {
	vkDeviceWaitIdle(device);
}