#pragma once

#include <vulkan/vulkan.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "core/Memory/memory.h"
#include "core/renderer_core.h"

#include "stdErr.h"

class Renderer {
private:
	mem::stack stash;

	VkInstance instance = VK_NULL_HANDLE;

	struct PhysicalDevice {
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceMemoryProperties memory{};
		VkPhysicalDeviceFeatures features{};

		VkPhysicalDevice handle = VK_NULL_HANDLE;
	};

	mem::span<PhysicalDevice> physicalDevices;
	PhysicalDevice* physicalDevice;

	VkSurfaceKHR surface = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;

	struct QueueFamilyIndices {
		uint32_t graphics;
		uint32_t transfer;
		uint32_t present;
	} familyIndex;

	struct DeviceQueues {
		VkQueue graphics;
		VkQueue transfer;
		VkQueue present;
	} deviceQueue;

	VmaAllocator allocator = VK_NULL_HANDLE;

	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VmaAllocation stagingAllocation;
	rndr::TransferStage transferStage;



	rndr::PresentationStage presentationStage;


	std::vector<size_t> renderTargets;

	std::vector<VkImage> images;
	std::vector<VkImageView> presentationPool;

public:
	Renderer() noexcept = default;
	~Renderer() noexcept;

	std::error_code deploy() noexcept;

	void enumeratePhysicalDeviceNames(size_t* count, const char** const deviceNames) noexcept;
	std::error_code createDevice(HINSTANCE hinstance, HWND hwnd, size_t physicalDeviceIndex);

	int pushRenderTarget(HINSTANCE hinstance, HWND hwnd) noexcept;

	int createImage(const void* const pData, size_t _Size, uint32_t _Width, uint32_t _Height) noexcept;

	void reset() noexcept;

};