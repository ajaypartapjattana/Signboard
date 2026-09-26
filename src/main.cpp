#include <iostream>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

#include <core/memory.h>
#include <core/io/io.h>

#include <Input/input.h>
#include <Platform/display.h>
#include <Renderer/renderer.h>

int loadPng(mem_stack* const pScratch, const char* _Path) noexcept {
	do {
		int error;
		
		size_t fileSize = 0;
		error = io::getBinarySize(_Path, &fileSize);
		
		if (error)
			return -1;
		
		pScratch->frame();

		uint8_t* imageBin = mem_stackAllocateRange<uint8_t>(pScratch, fileSize);

		if (!imageBin)
			break;

		error = io::loadBinary(_Path, fileSize, imageBin);

		if (error)
			break;

		io::ImageInfo imageInfo;
		error = io::fetchPngInfo(imageBin, fileSize, &imageInfo);

		if (error)
			break;

		const size_t imageSize = io::getImageSize(&imageInfo);
		
		uint8_t* image = mem_allocateSizeRange<uint8_t>(imageSize);

		if (!image)
			break;

		{
			io::ImageDecodeInfo decodeInfo{};
			decodeInfo.imageInfo = &imageInfo;
			decodeInfo.bin = imageBin;
			decodeInfo.binSize = fileSize;
			decodeInfo.pDst = image;

			error = io::decodePng(pScratch, &decodeInfo);
		}

		mem_freeSizeRange<uint8_t>(image);

		if (error)
			break;

		pScratch->restore();

		return 0;
	} while (false);

	pScratch->restore();

	return -1;
}

int main() {
	mem_stack scratch;

	if (scratch.create(16u << 20))
		return EXIT_FAILURE;

	InputDeviceSet inputDevice;

	DisplayContext windowCtx;
	DisplayWindow window;
	EventBuffer eventBuffer;

	VulkanContext vulkanCtx;
	__Renderer renderDevice;

	Collection collection;
	Scene scene;
	Camera camera;
	Texture texture;

	float aspect;

	InputKeyField keyField{};
	InputDragField dragField{};

	DisplayCursor cursor{};

	{
		uint32_t deviceCount;

		InputDeviceDicoverControlInfo discoverInfo{};
		discoverInfo.maxInputDevice = 10u;

		if (discoverInputDevices(&discoverInfo, &deviceCount, &inputDevice))
			goto cleanup_0;

		scratch.frame();

		mem_span<const char*> deviceName;

		mem_stackAllocateSpan(&scratch, &deviceName, (size_t)deviceCount);

		if (!deviceName.count) {
			scratch.restore();
			goto cleanup_1;
		}

		enumerateInputDeviceName(inputDevice, deviceCount, deviceName.data);

		const char** const pNameEnd = deviceName.end();
		for (const char** pName{ deviceName.data }; pName != pNameEnd; ++pName)
			printf("input_device : {%s}\n", *pName);

		printf("\n");
			
		scratch.restore();
	}

	{
		if (requestDisplayContext(&windowCtx))
			goto cleanup_1;

		{
			WindowCreateInfo createInfo{};
			createInfo.flags = WINDOW_CREATE_FULLSCREEN_BIT | WINDOW_CREATE_RESIZABLE_BIT;
			createInfo.width = 800u;
			createInfo.height = 800u;
			createInfo.x = 0u;
			createInfo.y = 0u;
			createInfo.title = "My Window";

			if (createDisplayWindow(windowCtx, &createInfo, &window))
				goto cleanup_2;
		}

		aspect = getWindowAspect(window);

		EventBufferCreateInfo createInfo{};
		createInfo.size = 64u;
			
		if (createEventBuffer(&createInfo, &eventBuffer))
			goto cleanup_3;
	}

	{
		if (createVulkanContext(&scratch, &vulkanCtx))
			goto cleanup_4;

		uint32_t deviceCount;
		getPhysicalDeviceCount(vulkanCtx, &deviceCount);
		
		if (!deviceCount) {
			printf("FAILURE : no_vulkan_support.\n");
			goto cleanup_5;
		}

		int device = -1;

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
			goto cleanup_5;
		}

		const char* deviceName;
		getPhysicalDeviceName(vulkanCtx, device, &deviceName);

		printf("graphics_device : {%s}\n", deviceName);
		printf("compute_device : {%s}\n", deviceName);

		printf("\n");

		VulkanSurfaceDependencyInfo surfaceInfo;
		getVulkanSurfaceDependencyInfo(windowCtx, window, &surfaceInfo);

		EmulatorCreateInfo createInfo{};
		createInfo.windowContext = surfaceInfo.context;
		createInfo.windowHandle = surfaceInfo.window;
		createInfo.physicalDevice = device;
		createInfo.imageCount = 2u;
		createInfo.renderProcess = 2u;
		createInfo.transferProcess = 3u;
		createInfo.stageCapacity = (size_t)16u << 20;

		if (createRenderDevice(vulkanCtx, &createInfo, &scratch, &renderDevice))
			goto cleanup_5;
	}

	raiseDisplayWindow(windowCtx, window);

	{
		const Vertex vertexData[] = {
			{ { -0.5f, -0.5f }, { 0.0f, 0.0f } },
			{ {  0.5f, -0.5f }, { 1.0f, 0.0f } },
			{ {  0.5f,  0.5f }, { 1.0f, 1.0f } },
			{ { -0.5f,  0.5f }, { 0.0f, 1.0f } }
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

		if (createCollection(renderDevice, &createInfo, &collection))
			goto cleanup_6;
	}

	{
		SceneCreateInfo createInfo{};
		createInfo.collection = collection;
		createInfo.instanceCount = 4u;
		createInfo.drawCount = 10u;

		if (createScene(renderDevice, &createInfo, &scratch, &scene))
			goto cleanup_7;
	}

	{
		const InstanceData data[1] = { glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) };

		ObjectInstance instance{};
		instance.model = 0u;
		instance.instanceCount = 1u;
		instance.pInstances = data;

		if (pushObjectInstance(renderDevice, scene, &instance))
			goto cleanup_8;
	}

	{
		CameraCreateInfo createInfo{};
		createInfo.bindings = 2u;

		if (createCamera(renderDevice, &createInfo, &scratch, &camera))
			goto cleanup_8;
	}

	{
		CameraData data[2]{};
		data[0].view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		data[0].projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		data[0].projection[1][1] *= -1;

		data[1].view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		data[1].projection = glm::ortho(-10.0f, 10.0f, -5.0f, 5.0f, 0.1f, 100.0f);
		data[1].projection[1][1] *= -1;

		CameraWrite write{};
		write.firstCamera = 0u;
		write.count = 2u;
		write.pData = data;

		updateCamera(renderDevice, camera, &write);
	}

	{
		TextureCreateInfo createInfo{};
		createInfo.path = "assets/textures/seaside.png";

		if (createTexture(renderDevice, &createInfo, &scratch, &texture))
			goto cleanup_9;
	}

	if (beginInputEventPoll(inputDevice))
		goto cleanup_10;

	while (true) {
		WindowStateField events = 0;

		while (pollWindowEvents(windowCtx, eventBuffer))
			resolveWindowEvents(eventBuffer, window, &events, &cursor);

		if (events & WINDOW_STATE_TERMINATION_IMMINENT_BIT) {
			while(waitRenderDevice(renderDevice));
			break;
		}
			
		if (events & WINDOW_STATE_EXTENT_DIRTY_BIT) {
			if (configureRenderDevice(renderDevice)) {
				while(waitRenderDevice(renderDevice));
				break;
			}

			aspect = getWindowAspect(window);

			events &= ~WINDOW_STATE_EXTENT_DIRTY_BIT;
		}

		if (events & WINDOW_STATE_MINIMIZED_BIT) {
			if (waitWindowEvents(windowCtx, eventBuffer))
				resolveWindowEvents(eventBuffer, window, &events, &cursor);
			else
				break;
		}

		{
			pollInputs(inputDevice, &dragField, &keyField);

			if (isKeyDown(&keyField, INPUT_KEY_W)) {
				printf("\r\033[Kx = %d, y = %d", cursor.x, cursor.y);
				fflush(stdout);
			}
		}
		
		if (events & WINDOW_STATE_POINTER_MOTION_BIT)
			events &= ~WINDOW_STATE_POINTER_MOTION_BIT;

		if (isKeyPressed(&keyField, INPUT_KEY_R)) {
			dragField.delta[0] = 0u;
			dragField.delta[1] = 0u;
		}

		{
			CameraData data[2]{};
			data[0].view = glm::lookAt(glm::vec3((float)dragField.delta[0] / 100.0f, -(float)dragField.delta[1] / 100.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			data[0].projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
			data[0].projection[1][1] *= -1;

			data[1].view = glm::lookAt(glm::vec3((float)dragField.delta[0] / 100.0f, -(float)dragField.delta[1] / 100.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			data[1].projection = glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, 0.1f, 100.0f);
			data[1].projection[1][1] *= -1;

			CameraWrite write{};
			write.firstCamera = 0u;
			write.count = 2u;
			write.pData = data;

			updateCamera(renderDevice, camera, &write);
		}

		switch (beginFrame(renderDevice)) {
		case 0:
			break;
		case 1:
			events |= WINDOW_STATE_EXTENT_DIRTY_BIT;
			continue;
		default:
			while(waitRenderDevice(renderDevice));
			goto cleanup_11;
		}

		render(renderDevice, collection, scene, texture, camera, isKeyDown(&keyField, INPUT_KEY_NUMPAD_0) ? 1u : 0u);

		switch (endFrame(renderDevice)) {
		case 0:
			break;
		case 1:
			events |= WINDOW_STATE_EXTENT_DIRTY_BIT;
			break;
		default:
			while(waitRenderDevice(renderDevice));
			goto cleanup_11;
		}
	}

cleanup_11:
	endInputEventPoll(inputDevice);

cleanup_10:
	destroyTexture(renderDevice, texture);

cleanup_9:
	destroyCamera(renderDevice, camera);

cleanup_8:
	destroyScene(renderDevice, scene);

cleanup_7:
	destroyCollection(renderDevice, collection);

cleanup_6:
	destroyRenderDevice(vulkanCtx, renderDevice);

cleanup_5:
	destroyVulkanContext(vulkanCtx);

cleanup_4:
	destroyEventBuffer(eventBuffer);

cleanup_3:
	destroyDisplayWindow(windowCtx, window);

cleanup_2:
	destroyDisplayContext(windowCtx);

cleanup_1:
	destroyInputDeviceSet(inputDevice);

cleanup_0:
	scratch.reset();

	return 0;
}