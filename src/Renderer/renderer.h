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

struct RenderDevice_T;
using RenderDevice = RenderDevice_T*;

int createRenderDevice(VulkanContext const context, const EmulatorCreateInfo* const pCreateInfo, mem_stack* const pScratch, RenderDevice* const pRenderer) noexcept;
void destroyRenderDevice(VulkanContext const context, RenderDevice const renderer) noexcept;

int waitRenderDevice(const RenderDevice renderer) noexcept;

int configureRenderDevice(const RenderDevice renderer) noexcept;

struct GeometryPage_T;
using GeometryPage = GeometryPage_T*;

enum PageAccessType : uint32_t {
	PAGE_ACCESS_NONE,
	PAGE_ACCESS_SEQUENTIAL,
	PAGE_ACCESS_RANDOM
};

struct GeometryPageCreateInfo {
	PageAccessType access;
	uint32_t modelCap;
	size_t vertexCap;
	size_t IndexCap;
};

int createGeometryPage(RenderDevice renderer, const GeometryPageCreateInfo* pCreateInfo, GeometryPage* pCollection) noexcept;
void destroyGeometryPage(RenderDevice renderer, GeometryPage _Collection) noexcept;

struct Vertex {
	glm::vec3 pos;
	glm::vec2 uv;
};

using Index = uint32_t;

struct ModelAppendInfo {
	const Vertex* pVertex;
	const Index* pIndex;
	uint32_t vertexCount;
	uint32_t indexCount;
};

struct ModelUpdateInfo {
	const Vertex* pVertex;
	const Index* pIndex;
	uint32_t firstVertex;
	uint32_t firstIndex;
	uint32_t vertexCount;
	uint32_t indexCount;
	uint32_t modelIndex;
};

struct GeometryWriteInfo {
	uint32_t appendCount;
	uint32_t updateCount;
	const ModelAppendInfo* pAppendInfo;
	const ModelUpdateInfo* pUpdateInfo;
};

int writeGeometryPage(RenderDevice renderer, GeometryPage page, const GeometryWriteInfo* pWriteInfo) noexcept;

struct Texture_T;
using Texture = Texture_T*;

struct TextureCreateInfo {
	const char* path;
};

int createTexture(RenderDevice renderer, const TextureCreateInfo* pCreateInfo, mem_stack* pScratch, Texture* pTexture) noexcept;
void destroyTexture(RenderDevice renderer, Texture texture) noexcept;

struct Scene_T;
using Scene = Scene_T*;

struct SceneCreateInfo {
	GeometryPage geometry;
	uint32_t instanceCount;
	uint32_t drawCount;
};

int createScene(RenderDevice renderer, const SceneCreateInfo* pCreateInfo, mem_stack* pScratch, Scene* pScene) noexcept;
void destroyScene(RenderDevice renderer, Scene scene) noexcept;

using Transform = glm::mat4;

struct InstanceData {
	Transform local;
};

struct ObjectInstance {
	uint32_t model;
	uint32_t instanceCount;
	const InstanceData* pInstances;
};

int pushObjectInstance(RenderDevice renderer, Scene _Scene, const ObjectInstance* pObject) noexcept;

struct Camera_T;
using Camera = Camera_T*;

struct CameraCreateInfo {
	uint32_t bindings;
};

int createCamera(RenderDevice renderer, const CameraCreateInfo* pCreateInfo, mem_stack* pScratch, Camera* pCamera) noexcept;
void destroyCamera(RenderDevice renderer, Camera camera) noexcept;

struct CameraData {
	glm::mat4 view;
	glm::mat4 projection;
};

struct CameraWrite {
	uint32_t firstCamera;
	uint32_t count;
	const CameraData* pData;
};

struct DrawInfo	{
	GeometryPage geometry;
	Scene scene;
	Texture texture;
	uint32_t cameraIndex;
};

enum DrawSequenceRecordFlag : uint32_t {
	SEQUENCE_RECORD_STATIC,
	SEQUENCE_RECORD_DYNAMIC
};

int recordDrawSequence(RenderDevice renderer, Camera camera, uint32_t drawCount, const DrawInfo* pDrawInfo) noexcept;

int beginFrame(RenderDevice renderer) noexcept;
void updateCamera(RenderDevice renderer, Camera _Camera, const CameraWrite* pWrite) noexcept;

void render(RenderDevice renderer, GeometryPage collection, Scene scene, Texture texture, Camera camera, uint32_t cameraIndex) noexcept;

int endFrame(RenderDevice renderer) noexcept;
