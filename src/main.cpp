#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include <core/Memory/memory.h>
#include <core/io/io.h>

#include <Platform/platform.h>
#include <Renderer/renderer.h>

int readPng(mem::stack* const pScratch, const char* _Path) noexcept {
	const mem::marker mark = pScratch->mark();
	
	io::Inflator _inflator = nullptr;

	do {
		int error;

		size_t fileSize = 0;
		error = io::getBinarySize(_Path, &fileSize);

		if (error)
			break;

		mem::span<uint8_t> imageBin = pScratch->alloc<uint8_t>(fileSize);

		error = io::loadBinary(_Path, fileSize, imageBin);

		if (error)
			break;;

		io::ImageInfo imageInfo;
		error = io::fetchPngInfo(imageBin, fileSize, &imageInfo);

		if (error)
			break;

		const size_t imageSize = io::getImageSize(&imageInfo);
		mem::span<uint8_t> image = pScratch->alloc<uint8_t>(imageSize);
		
		io::Inflator inflator = nullptr;

		{
			size_t resolveMemorySize;
			io::getInflateBufferSize(&imageInfo, &resolveMemorySize);
			mem::span<uint8_t> resolveMemory = pScratch->alloc<uint8_t>(resolveMemorySize);

			io::InflatorCreateInfo createInfo{};
			createInfo.imageInfo = &imageInfo;
			createInfo.pStreamSrc = imageBin.pBegin;
			createInfo.StreamSize = imageBin.size();
			createInfo.pDst = image.pBegin;
			createInfo.pLimit = image.pEnd;
		
			error = io::createInflator(&createInfo, resolveMemory, &inflator);
		}

		if (error)
			break;

		error = io::decodePng(inflator);

		if (error)
			break;

		io::destroyInflator(_inflator);

		pScratch->restore(mark);

		return 0;
	} while (false);

	if (_inflator)
		io::destroyInflator(_inflator);
	
	pScratch->restore(mark);

	return -1;
}

using FlowControl = uint32_t;
enum FlowControlBits : FlowControl {
	FLOW_CONTROL_HALT_BIT = 1u << 0,
	FLOW_CONTROL_DIRTY_EXTENT_BIT = 1u << 1,
	FLOW_CONTROL_WAIT_BIT = 1u << 2
};

int main() {
	mem::stack scratch;

	try {
		scratch.resize(32u << 20);
	}
	catch (const std::exception* _Except) {
		return EXIT_FAILURE;
	}

	DisplayContext windowCtx = nullptr;
	DisplayWindow window = nullptr;
	EventBuffer eventBuffer = nullptr;

	VulkanContext vulkanCtx = nullptr;
	Emulator emulator = nullptr;

	Surface surface = nullptr;
	Renderer renderer = nullptr;
	Loader loader = nullptr;

	RenderPass renderPass = nullptr;

	Collection collection = nullptr;
	Scene scene = nullptr;
	Camera camera = nullptr;
	
	do {
		int failure;

		failure = requestDisplayContext(&windowCtx);

		if (failure)
			break;

		{
			WindowCreateInfo createInfo{};
			createInfo.flags = WINDOW_CREATE_FULLSCREEN_BIT | WINDOW_CREATE_RESIZABLE_BIT;
			createInfo.width = 800u;
			createInfo.height = 800u;
			createInfo.x = 0;
			createInfo.title = "My Window";

			failure = createDisplayWindow(windowCtx, &createInfo, &window);
		}

		if (failure)
			break;

		{
			EventBufferCreateInfo createInfo{};
			createInfo.eventMask = WINDOW_EVENT_ALL_BIT;
			createInfo.size = 64u;
			
			failure = createEventBuffer(&createInfo, &eventBuffer);
		}

		if (failure)
			break;

		failure = requestVulkanContext(&scratch, &vulkanCtx);

		if (failure)
			break;

		uint32_t selectedDeviceIndex = 0;

		{
			uint32_t deviceCount;
			enumeratePhysicalDevices(vulkanCtx, &deviceCount, nullptr);

			mem::span<const char*> devices = scratch.alloc<const char*>(deviceCount);
			enumeratePhysicalDevices(vulkanCtx, &deviceCount, devices.pBegin);

			for (size_t i = 0; i < deviceCount; ++i) {
				std::cout << "[" << i << "] : " << devices[i] << std::endl;
			}

			std::cout << "PHYSICAL_DEVICE_INDEX : ";
			std::cin >> selectedDeviceIndex;
		}

		raiseDisplayWindow(windowCtx, window);

		{
			VulkanSurfaceDependencyInfo surfaceInfo;
			getVulkanSurfaceDependencyInfo(windowCtx, window, &surfaceInfo);

			EmulatorCreateInfo createInfo{};
			createInfo.physicalDevice = selectedDeviceIndex;
			createInfo.windowContext = surfaceInfo.context;
			createInfo.windowHandle = surfaceInfo.window;

			failure = createEmulator(vulkanCtx, &createInfo, &scratch, &emulator);
		}
			
		if (failure)
			break;

		{
			SurfaceCreateInfo createInfo{};
			createInfo.minImageCount = 2u;

			failure = createSurface(emulator, &createInfo, &scratch, &surface);
		}

		if (failure)
			break;

		{
			RendererCreateInfo createInfo{};
			createInfo.maxRenderProcess = 2u;

			failure = createRenderer(emulator, &createInfo, &renderer);
		}
		
		if (failure)
			break;

		{
			LoaderCreateInfo createInfo{};
			createInfo.stageSize = 16ull << 20;
			createInfo.maxLoadProcess = 2u;

			failure = createLoader(emulator, &createInfo, &loader);
		}

		if (failure)
			break;

		{
			RenderPassCreateInfo createInfo{};
			createInfo.surface = surface;

			failure = createRenderPass(emulator, &createInfo, &scratch, &renderPass);
		}
		
		if (failure)
			break;

		{
			const Vertex vertexData[] = {
				{ { -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
				{ {  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
				{ {  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
				{ { -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } }
			};

			const Index indexData[] = {
				0, 1, 2, 2, 3, 0
			};

			ModelInfo model[1]{};
			model[0].vertexCount = 4u;
			model[0].pVertex = vertexData;
			model[0].indexCount = 6u;
			model[0].pIndex = indexData;

			CollectionCreateInfo createInfo{};
			createInfo.modelCount = 1u;
			createInfo.pModelInfos = model;

			failure = createCollection(emulator, loader, &createInfo, nullptr, &collection);
		}

		if (failure)
			break;

		failure = waitLoader(loader);

		if (failure)
			break;

		{
			SceneCreateInfo createInfo{};
			createInfo.collection = collection;
			createInfo.renderBox = renderPass;
			createInfo.instanceCount = 4u;
			createInfo.drawCount = 10u;

			failure = createScene(emulator, renderer, &createInfo, &scratch, &scene);
		}

		if (failure)
			break;

		{
			const InstanceData data[1] = { glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) };

			ObjectInstance instance{};
			instance.model = 0u;
			instance.instanceCount = 1u;
			instance.pInstances = data;

			failure = pushObjectInstance(scene, &instance);
		}

		if (failure)
			break;

		{
			CameraCreateInfo createInfo{};
			createInfo.renderBox = renderPass;
			createInfo.bindings = 2u;

			failure = createCamera(emulator, renderer, &createInfo, &scratch, &camera);
		}

		if (failure)
			break;

		{
			CameraData data{};
			data.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			data.projection = glm::perspective(glm::radians(78.0f), getWindowAspect(window), 0.1f, 10.0f);
			data.projection[1][1] *= -1;

			CameraWrite write{};
			write.firstCamera = 0u;
			write.count = 1u;
			write.pData = &data;

			updateCamera(camera, &write);
		}

		FlowControl control = 0;

		while (true) {
			WindowEventFlags events = 0;

			while (pollWindowEvents(windowCtx, eventBuffer))
				resolveWindowEvents(eventBuffer, window, &events);

			if (events) {
				control |= events & WINDOW_EVENT_CLOSE_BIT ? FLOW_CONTROL_HALT_BIT : 0u;
				control |= events & WINDOW_EVENT_RESIZED_BIT ? FLOW_CONTROL_DIRTY_EXTENT_BIT : 0u;
				control |= events & WINDOW_EVENT_MINIMIZED_BIT ? FLOW_CONTROL_WAIT_BIT : 0u;			
			}

			if (control & FLOW_CONTROL_HALT_BIT)
				break;
			
			if (control & FLOW_CONTROL_DIRTY_EXTENT_BIT) {
				failure = updateSurface(surface);

				if (failure)
					break;

				{
					RenderPassUpdateInfo updateInfo{};
					updateInfo.surface = surface;

					failure = updateRenderPass(renderPass, &updateInfo);
				}

				if (failure)
					break;

				{
					CameraData data{};
					data.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					data.projection = glm::perspective(glm::radians(78.0f), getWindowAspect(window), 0.1f, 10.0f);
					data.projection[1][1] *= -1;

					CameraWrite write{};
					write.firstCamera = 0u;
					write.count = 1u;
					write.pData = &data;

					updateCamera(camera, &write);
				}

				control &= ~FLOW_CONTROL_DIRTY_EXTENT_BIT;
			}

			if (control & FLOW_CONTROL_WAIT_BIT) {
				if (waitWindowEvents(windowCtx, eventBuffer))
					resolveWindowEvents(eventBuffer, window, &events);
				else
					break;

				control &= ~FLOW_CONTROL_WAIT_BIT;
			}

			failure = beginFrame(renderer, surface);

			if (failure) {
				if (failure < 0)
					break;
					
				control |= FLOW_CONTROL_DIRTY_EXTENT_BIT;
				continue;
			}

			beginRenderPass(renderer, renderPass, camera);

			setActiveCamera(renderer, 0u);
			render(renderer, collection, scene);

			endPass(renderer);

			failure = endFrame(renderer, surface);

			if (failure)
				break;

			failure = presentFrame(renderer, surface);

			if (failure) {
				if (failure < 0)
					break;

				control |= FLOW_CONTROL_DIRTY_EXTENT_BIT;
			}
		}

		if (failure)
			break;

		while (waitRenderer(renderer));
		while (waitSurface(surface));

		while (waitLoader(loader));

		destroyCamera(camera);
		destroyScene(scene);
		destroyCollection(collection);
		destroyLoader(loader);

		destroyRenderer(renderer);
		destroyRenderPass(renderPass);
		destroySurface(surface);

		destroyEmulator(emulator);
		destroyVulkanContext(vulkanCtx);

		destroyEventBuffer(eventBuffer);
		destroyDisplayWindow(windowCtx, window);
		destroyDisplayContext(windowCtx);

		return EXIT_SUCCESS;
		
	} while (false);

	if (emulator)
		while (waitEmulator(emulator));

	if (camera)
		destroyCamera(camera);

	if (scene)
		destroyScene(scene);

	if (collection)
		destroyCollection(collection);

	if (loader)
		destroyLoader(loader);

	if (renderer)
		destroyRenderer(renderer);

	if (renderPass)
		destroyRenderPass(renderPass);

	if (surface)
		destroySurface(surface);

	if (emulator)
		destroyEmulator(emulator);
	
	if (vulkanCtx)
		destroyVulkanContext(vulkanCtx);

	if (eventBuffer)
		destroyEventBuffer(eventBuffer);

	if (window)
		destroyDisplayWindow(windowCtx, window);

	if (windowCtx)
		destroyDisplayContext(windowCtx);

	return EXIT_FAILURE;
}