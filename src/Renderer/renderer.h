#pragma once

#include <glm/glm.hpp>

#include <core/memory.h>

struct VulkanContext_T;
using VulkanContext = VulkanContext_T*;

int createVulkanContext(mem_stack* pScratch, VulkanContext* pContext) noexcept;
void destroyVulkanContext(VulkanContext context) noexcept;

void getPhysicalDeviceCount(VulkanContext context, uint32_t* pCount) noexcept;
void queryPerformaceOptimalDevice(VulkanContext context, uint32_t minIndex, int* pIndex) noexcept;
void queryBatteryOptimalDevice(VulkanContext context, uint32_t minIndex, int* pIndex) noexcept;

void getPhysicalDeviceName(VulkanContext context, uint32_t index, const char** pName) noexcept;
void enumeratePhysicalDeviceName(VulkanContext context, uint32_t minIndex, uint32_t count, const char** pDeviceNames) noexcept;

struct EmulatorCreateInfo {
	void* windowContext;
	uintptr_t windowHandle;
	uint32_t physicalDevice;
	uint32_t imageCount;
	uint32_t renderProcess;
	uint32_t transferProcess;
	size_t stageCapacity;
};

struct __Renderer_T;
using __Renderer = __Renderer_T*;

int createRenderDevice(VulkanContext const context, const EmulatorCreateInfo* const pCreateInfo, mem_stack* const pScratch, __Renderer* const pRenderer) noexcept;
void destroyRenderDevice(VulkanContext const context, __Renderer const renderer) noexcept;

int waitRenderDevice(const __Renderer renderer) noexcept;

int configureRenderDevice(const __Renderer renderer) noexcept;

struct Collection_T;
using Collection = Collection_T*;

struct Vertex {
	glm::vec2 pos;
	glm::vec2 uv;
};

using Index = uint32_t;

struct ModelInfo {
	const Vertex* pVertex;
	size_t vertexCount;
	const Index* pIndex;
	size_t indexCount;
};

struct CollectionCreateInfo {
	uint32_t modelCount;
	const ModelInfo* pModelInfos;
};

int createCollection(__Renderer renderer, const CollectionCreateInfo* pCreateInfo, Collection* pCollection) noexcept;
void destroyCollection(__Renderer renderer, Collection _Collection) noexcept;

struct Texture_T;
using Texture = Texture_T*;

struct TextureCreateInfo {
	const char* path;
};

int createTexture(__Renderer renderer, const TextureCreateInfo* pCreateInfo, mem_stack* pScratch, Texture* pTexture) noexcept;
void destroyTexture(__Renderer renderer, Texture texture) noexcept;

struct Scene_T;
using Scene = Scene_T*;

struct SceneCreateInfo {
	Collection collection;
	uint32_t instanceCount;
	uint32_t drawCount;
};

int createScene(__Renderer renderer, const SceneCreateInfo* pCreateInfo, mem_stack* pScratch, Scene* pScene) noexcept;
void destroyScene(__Renderer renderer, Scene scene) noexcept;

using Transform = glm::mat4;

struct InstanceData {
	Transform local;
};

struct ObjectInstance {
	uint32_t model;
	uint32_t instanceCount;
	const InstanceData* pInstances;
};

int pushObjectInstance(__Renderer renderer, Scene _Scene, const ObjectInstance* pObject) noexcept;

struct Camera_T;
using Camera = Camera_T*;

struct CameraCreateInfo {
	uint32_t bindings;
};

int createCamera(__Renderer renderer, const CameraCreateInfo* pCreateInfo, mem_stack* pScratch, Camera* pCamera) noexcept;
void destroyCamera(__Renderer renderer, Camera camera) noexcept;

struct CameraData {
	glm::mat4 view;
	glm::mat4 projection;
};

struct CameraWrite {
	uint32_t firstCamera;
	uint32_t count;
	const CameraData* pData;
};

void updateCamera(__Renderer renderer, Camera _Camera, const CameraWrite* pWrite) noexcept;

int beginFrame(__Renderer renderer) noexcept;

void render(__Renderer renderer, Collection collection, Scene scene, Texture texture, Camera camera, uint32_t cameraIndex) noexcept;

int endFrame(__Renderer renderer) noexcept;
