#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include <core/Memory/memory.h>
#include <core/io/io.h>

#include <Input/input.h>
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

int main() {
	int failure{};
	
	mem::stack scratch;

	try {
		scratch.resize(16u << 20);
	}
	catch (const std::exception* _Except) {
		return EXIT_FAILURE;
	}

	InputDeviceSet inputDevice = nullptr;

	do {
		uint32_t deviceCount;

		{
			InputDeviceDicoverControlInfo discoverInfo{};
			discoverInfo.flags = INPUT_DEVICE_CAPABILITY_KEYBOARD_BIT | INPUT_DEVICE_CAPABILITY_MOUSE_BIT;
			discoverInfo.maxInputDevice = 10u;

			failure = discoverInputDevices(&discoverInfo, &deviceCount, &inputDevice);
		}

		if (failure)
			break;

		scratch.mark();

		mem::span<const char*> deviceName = scratch.alloc<const char*>((size_t)deviceCount);

		if (!deviceName) {
			scratch.restore();
			break;
		}

		enumerateInputDeviceName(inputDevice, deviceCount, deviceName);

		const char** const pNameEnd = deviceName.pEnd;
		for (const char** pName{ deviceName.pBegin }; pName != pNameEnd; ++pName)
			printf("input_device : {%s}\n", *pName);

		printf("\n");
			
		scratch.restore();

	} while (false);

	if (failure)
		return EXIT_FAILURE;

	DisplayContext windowCtx = nullptr;
	DisplayWindow window = nullptr;
	EventBuffer eventBuffer = nullptr;

	do {
		failure = requestDisplayContext(&windowCtx);

		if (failure)
			break;

		{
			WindowCreateInfo createInfo{};
			createInfo.flags = WINDOW_CREATE_FULLSCREEN_BIT | WINDOW_CREATE_RESIZABLE_BIT;
			createInfo.width = 800u;
			createInfo.height = 800u;
			createInfo.x = 0u;
			createInfo.y = 0u;
			createInfo.title = "My Window";

			failure = createDisplayWindow(windowCtx, &createInfo, &window);
		}

		if (failure)
			break;

		{
			EventBufferCreateInfo createInfo{};
			createInfo.size = 64u;
			
			failure = createEventBuffer(&createInfo, &eventBuffer);
		}

	} while (false);

	if (failure) {
		if (eventBuffer)
			destroyEventBuffer(eventBuffer);

		if (window)
			destroyDisplayWindow(windowCtx, window);

		destroyDisplayContext(windowCtx);

		return EXIT_FAILURE;
	}

	VulkanContext vulkanCtx = nullptr;
	Emulator emulator = nullptr;

	do {
		failure = requestVulkanContext(&scratch, &vulkanCtx);

		if (failure)
			break;
			
		uint32_t deviceCount;
		getPhysicalDeviceCount(vulkanCtx, &deviceCount);
		
		if (!deviceCount)
			break;

		int device;

		int powerOptimal = -1;
		queryBatteryOptimalDevice(vulkanCtx, 0u, &powerOptimal);

		if (powerOptimal != -1)
			device = powerOptimal;

		int performaceOptimal = -1;
		queryPerformaceOptimalDevice(vulkanCtx, 0u, &performaceOptimal);

		if (performaceOptimal != -1)
			device = performaceOptimal;

		if (device == -1) {
			printf("FAILURE : no_optimal_device_found.\n");
			break;
		}

		const char* deviceName;
		getPhysicalDeviceName(vulkanCtx, device, &deviceName);

		printf("graphics_device : {%s}\n", deviceName);
		printf("compute_device : {%s}\n", deviceName);

		printf("\n");

		{
			VulkanSurfaceDependencyInfo surfaceInfo;
			getVulkanSurfaceDependencyInfo(windowCtx, window, &surfaceInfo);

			EmulatorCreateInfo createInfo{};
			createInfo.physicalDevice = device;
			createInfo.windowContext = surfaceInfo.context;
			createInfo.windowHandle = surfaceInfo.window;

			failure = createEmulator(vulkanCtx, &createInfo, &scratch, &emulator);
		}
			
	} while (false);

	if (failure) {
		if (emulator)
			destroyEmulator(emulator);
			
		destroyVulkanContext(vulkanCtx);

		destroyEventBuffer(eventBuffer);
		destroyDisplayWindow(windowCtx, window);
		destroyDisplayContext(windowCtx);

		destroyInputDeviceSet(inputDevice);

		return EXIT_FAILURE;
	}

	raiseDisplayWindow(windowCtx, window);

	Surface surface = nullptr;
	Renderer renderer = nullptr;
	Loader loader = nullptr;	

	do {
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
	} while (false);

	if (failure) {
		if (loader)
			destroyLoader(loader);
		
		if (renderer)
			destroyRenderer(renderer);

		destroySurface(surface);

		destroyEmulator(emulator);
		destroyVulkanContext(vulkanCtx);

		destroyEventBuffer(eventBuffer);
		destroyDisplayWindow(windowCtx, window);
		destroyDisplayContext(windowCtx);

		destroyInputDeviceSet(inputDevice);

		return EXIT_FAILURE;
	}

	RenderPass renderPass = nullptr;
	Collection collection = nullptr;
	Scene scene = nullptr;
	Camera camera = nullptr;

	do {
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
			CameraData data[1]{};
			data[0].view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			data[0].projection = glm::perspective(glm::radians(78.0f), getWindowAspect(window), 0.1f, 10.0f);
			data[0].projection[1][1] *= -1;

			CameraWrite write{};
			write.firstCamera = 0u;
			write.count = 1u;
			write.pData = data;

			updateCamera(camera, &write);
		}

		failure = beginInputEventPoll(inputDevice);

	} while (false);

	

	if (failure) {
		if (camera)
			destroyCamera(camera);

		if (scene)
			destroyScene(scene);
			
		while (waitLoader(loader));

		if (collection)
			destroyCollection(collection);

		destroyRenderPass(renderPass);
		
		destroyLoader(loader);
		destroyRenderer(renderer);

		destroySurface(surface);

		destroyEmulator(emulator);
		destroyVulkanContext(vulkanCtx);

		destroyEventBuffer(eventBuffer);
		destroyDisplayWindow(windowCtx, window);
		destroyDisplayContext(windowCtx);

		destroyInputDeviceSet(inputDevice);

		return EXIT_FAILURE;
	}

	InputKeyEvent keyEvents[64u];

	InputState inputs{};
	inputs.key.pEvent = keyEvents;
	inputs.key.pNext = keyEvents;
	inputs.key.pEnd = keyEvents + 64u;
	inputs.key.modifier = 0u;

	while (true) {
		WindowStateFlags events = 0;

		while (pollWindowEvents(windowCtx, eventBuffer))
			resolveWindowEvents(eventBuffer, window, &events);

		if (events & WINDOW_STATE_TERMINATION_IMMINENT_BIT)
			break;
			
		if (events & WINDOW_STATE_EXTENT_DIRTY_BIT) {
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

			events &= ~WINDOW_STATE_EXTENT_DIRTY_BIT;
		}

		if (events & WINDOW_STATE_MINIMIZED_BIT) {
			if (waitWindowEvents(windowCtx, eventBuffer))
				resolveWindowEvents(eventBuffer, window, &events);
			else
				break;
		}

		{
			pollInputs(inputDevice, &inputs);

			inputs.cursor.posX += inputs.cursor.delX;
			inputs.cursor.posY += inputs.cursor.delY;

			inputs.cursor.delX = 0;
			inputs.cursor.delY = 0;

			const InputKeyEvent* const pKeyEventEnd = inputs.key.pNext;
			for (const InputKeyEvent* pKeyEvent{ keyEvents }; pKeyEvent != pKeyEventEnd; ++pKeyEvent) {
				if (pKeyEvent->key == INPUT_KEY_W)
					printf("x = %d, y = %d\n", inputs.cursor.posX, inputs.cursor.posY);
			}

			inputs.key.pNext = inputs.key.pEvent;
		}

		failure = beginFrame(renderer, surface);

		if (failure) {
			if (failure < 0)
				break;
					
			events |= WINDOW_STATE_EXTENT_DIRTY_BIT;
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

			events |= WINDOW_STATE_EXTENT_DIRTY_BIT;
		}
	}

	endInputEventPoll(inputDevice);

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

	destroyInputDeviceSet(inputDevice);

	if (failure)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}