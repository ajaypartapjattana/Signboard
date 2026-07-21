#pragma once

#include <vulkan/vulkan.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "core/Memory/memory.h"
#include "core/renderer_core.h"

class Renderer {
private:
	VkInstance instance = VK_NULL_HANDLE;

	struct PhysicalDevice {
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceMemoryProperties memory{};
		VkPhysicalDeviceFeatures features{};

		VkPhysicalDevice handle = VK_NULL_HANDLE;
	};
	std::vector<PhysicalDevice> m_physicalDevices;

	size_t activePhysicalDeviceIndex = 0;
	VkPhysicalDevice activePhysicalDevice = VK_NULL_HANDLE;

	VkDevice device = VK_NULL_HANDLE;

	struct {
		uint32_t graphicsFamilyIndex{};
		VkQueue graphicsQueue = VK_NULL_HANDLE;

		uint32_t transferFamilyIndex{};
		VkQueue transferQueue = VK_NULL_HANDLE;

		uint32_t presentFamilyIndex{};
		VkQueue presentQueue = VK_NULL_HANDLE;
	} execution;

	VmaAllocator allocator = VK_NULL_HANDLE;

	rndr::PresentationStage presentationStage;

	rndr::TransferStage transferStage;

	std::vector<size_t> renderTargets;

	std::vector<VkImage> images;
	std::vector<VkImageView> presentationPool;

public:
	Renderer() noexcept = default;
	~Renderer() noexcept;

	void deploy();

	void enumeratePhysicalDevices(size_t* count, const char** deviceNames) noexcept;
	void createDevice(HINSTANCE hinstance, HWND hwnd, size_t physicalDeviceIndex);

	int pushRenderTarget(HINSTANCE hinstance, HWND hwnd) noexcept;

	int createImage(const void* const pData, size_t _Size, uint32_t _Width, uint32_t _Height) noexcept;

	void reset() noexcept;

};