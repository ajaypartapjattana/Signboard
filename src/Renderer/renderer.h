#pragma once

#include <glm/glm.hpp>

#include <core/Memory/memory.h>

struct VulkanContext_T;
using VulkanContext = VulkanContext_T*;

int requestVulkanContext(mem::stack* pScratch, VulkanContext* pContext) noexcept;
void destroyVulkanContext(VulkanContext _Context) noexcept;

void getPhysicalDeviceCount(VulkanContext _Context, uint32_t* pCount) noexcept;
void queryPerformaceOptimalDevice(VulkanContext _Context, uint32_t minIndex, int* pIndex) noexcept;
void queryBatteryOptimalDevice(VulkanContext _Context, uint32_t minIndex, int* pIndex) noexcept;

void getPhysicalDeviceName(VulkanContext _Context, uint32_t index, const char** pName) noexcept;
void enumeratePhysicalDeviceName(VulkanContext _Context, uint32_t minIndex, uint32_t count, const char** pDeviceNames) noexcept;

struct EmulatorCreateInfo {
	void* windowContext;
	uintptr_t windowHandle;
	uint32_t physicalDevice;
};

struct Emulator_T;
using Emulator = Emulator_T*;

int createEmulator(VulkanContext _Context, const EmulatorCreateInfo* pCreateInfo, mem::stack* pScratch, Emulator* pEmulator) noexcept;
void destroyEmulator(Emulator _Emulator) noexcept;

int waitEmulator(Emulator const _Emulator) noexcept;

struct Loader_T;
using Loader = Loader_T*;

struct LoaderCreateInfo {
	size_t stageSize;
	uint32_t maxLoadProcess;
};

int createLoader(Emulator _Emulator, const LoaderCreateInfo* pCreateInfo, Loader* pLoader) noexcept;
void destroyLoader(Loader _Loader) noexcept;

int waitLoader(Loader _Loader) noexcept;

using ProcessCookie = uintptr_t;

struct Collection_T;
using Collection = Collection_T*;

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
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

int createCollection(Emulator _Emulator, Loader _AsynLoader, const CollectionCreateInfo* pCreateInfo, ProcessCookie* pProcessCookie, Collection* pScene) noexcept;
void destroyCollection(Collection _Scene) noexcept;

struct Surface_T;
using Surface = Surface_T*;

struct SurfaceCreateInfo {
	uint32_t minImageCount;
};

int createSurface(Emulator _Emulator, const SurfaceCreateInfo* pCreateInfo, mem::stack* pScratch, Surface* pCanvas) noexcept;
void destroySurface(Surface _Surface) noexcept;

int updateSurface(Surface _Surface) noexcept;

int waitSurface(Surface _Surface) noexcept;

struct Renderer_T;
using Renderer = Renderer_T*;

struct RendererCreateInfo {
	uint32_t maxRenderProcess;
};

int createRenderer(Emulator _Emulator, const RendererCreateInfo* pCreateInfo, Renderer* pRenderer) noexcept;
void destroyRenderer(Renderer _Renderer) noexcept;

int waitRenderer(Renderer _Renderer) noexcept;

struct RenderPass_T;
using RenderPass = RenderPass_T*;

struct RenderPassCreateInfo {
	Surface surface;
};

int createRenderPass(Emulator _Emulator, const RenderPassCreateInfo* pCreateInfo, mem::stack* pScratch, RenderPass* pRenderBox) noexcept;
void destroyRenderPass(RenderPass _RenderBox) noexcept;

struct RenderPassUpdateInfo {
	Surface surface;
};

int updateRenderPass(RenderPass _RenderPass, const RenderPassUpdateInfo* pUpdateInfo) noexcept;

struct Scene_T;
using Scene = Scene_T*;

struct SceneCreateInfo {
	RenderPass renderBox;
	Collection collection;
	uint32_t instanceCount;
	uint32_t drawCount;
};

int createScene(Emulator _Emulator, Renderer _Renderer, const SceneCreateInfo* pCreateInfo, mem::stack* pScratch, Scene* pScene) noexcept;
void destroyScene(Scene _Scene) noexcept;

using Transform = glm::mat4;

struct InstanceData {
	Transform local;
};

struct ObjectInstance {
	uint32_t model;
	uint32_t instanceCount;
	const InstanceData* pInstances;
};

int pushObjectInstance(Scene _Scene, const ObjectInstance* pObject) noexcept;

struct Camera_T;
using Camera = Camera_T*;

struct CameraCreateInfo {
	RenderPass renderBox;
	uint32_t bindings;
};

int createCamera(Emulator _Emulator, Renderer _Renderer, const CameraCreateInfo* pCreateInfo, mem::stack* pScratch, Camera* pCamera) noexcept;
void destroyCamera(Camera _Camera) noexcept;

struct CameraData {
	glm::mat4 view;
	glm::mat4 projection;
};

struct CameraWrite {
	uint32_t firstCamera;
	uint32_t count;
	const CameraData* pData;
};

void updateCamera(Camera const _Camera, const CameraWrite* const pWrite) noexcept;

int beginFrame(Renderer _Renderer, Surface _Surface) noexcept;

void beginRenderPass(Renderer _Renderer, RenderPass _RenderPass, Camera _Camera) noexcept;
void setActiveCamera(Renderer _Renderer, uint32_t camera) noexcept;
void render(Renderer _Renderer, Collection _Collection, Scene _Scene) noexcept;
void endPass(Renderer _Renderer) noexcept;

int endFrame(Renderer _Renderer, Surface _Surface) noexcept;
int presentFrame(Renderer _Renderer, Surface _Surface) noexcept;
