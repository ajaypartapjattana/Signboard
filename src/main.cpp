#include <stdio.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

#include <core/memory.h>
#include <core/io/io.h>

#include <Input/input.h>
#include <Platform/display.h>
#include <Renderer/renderer.h>

constexpr uint32_t sphereVertexDataRequirement(const uint32_t resolution) noexcept {
	return (resolution + 1u) * (resolution + 1u);
}

constexpr uint32_t sphereIndexDataRequirement(const uint32_t resolution) noexcept {
	return resolution * resolution * 6u;
}

constexpr void generateSphereVertexData(const uint32_t resolution, const float size, Vertex* const pVertex, Index* const pIndex) noexcept {
	const float radius = size * 0.5f;

	const uint32_t segments = resolution;
	const uint32_t rings = resolution;

	const float pi = glm::pi<float>();

	for (uint32_t y = 0; y <= rings; ++y) {
		const float v = static_cast<float>(y) / static_cast<float>(rings);
		const float phi = v * pi;

		const float sinPhi = std::sin(phi);
		const float cosPhi = std::cos(phi);

		for (uint32_t x = 0; x <= segments; ++x) {
			const float u = static_cast<float>(x) / static_cast<float>(segments);
			const float theta = u * (2.0f * pi);

			const float sinTheta = std::sin(theta);
			const float cosTheta = std::cos(theta);

			const uint32_t vertex = y * (segments + 1u) + x;

			pVertex[vertex] = { { radius * sinPhi * cosTheta, radius * cosPhi, radius * sinPhi * sinTheta }, { u, v } };
		}
	}

	uint32_t index = 0;

	for (uint32_t y = 0; y < rings; ++y) {
		for (uint32_t x = 0; x < segments; ++x) {
			const uint32_t a = y * (segments + 1u) + x;
			const uint32_t b = a + 1u;
			const uint32_t c = a + (segments + 1u);
			const uint32_t d = c + 1u;

			pIndex[index++] = a;
			pIndex[index++] = c;
			pIndex[index++] = b;

			pIndex[index++] = b;
			pIndex[index++] = c;
			pIndex[index++] = d;
		}
	}
}

constexpr void generateCubeVertexData(const float size, Vertex* const pVertex, Index* const pIndex) noexcept {
	constexpr glm::vec3 CUBE_VERTEX[] = {
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f,  0.5f }
	};

	constexpr Index CUBE_INDEX[] = {
		0, 2, 1,
		1, 2, 6,
		3, 5, 4,
		5, 7, 4,
		0, 3, 2,
		2, 3, 4,
		1, 6, 5,
		5, 6, 7,
		0, 1, 3,
		1, 5, 3,
		2, 4, 6,
		4, 7, 6
	};

	const glm::vec3* pPosData = CUBE_VERTEX;

	const Vertex* const pVertEnd = pVertex + 8u;
	for (Vertex* pVert = pVertex; pVert != pVertEnd; ++pVert)
		*pVert = { *pPosData++ * size, { 0.0f, 0.0f } };

	const Index* pIndexData = CUBE_INDEX;

	const Index* const pIndexEnd = pIndex + 36u;
	for (Index* pIdx = pIndex; pIdx != pIndexEnd; ++pIdx)
		*pIdx = *pIndexData++;
}


int main() {
	mem_stack scratch;

	if (scratch.create(32u << 20))
		return EXIT_FAILURE;

	InputDeviceSet inputDevice;

	DisplayContext windowCtx;
	DisplayWindow window;
	EventBuffer eventBuffer;

	VulkanContext vulkanCtx;
	RenderDevice renderDevice;

	GeometryPage geometry;
	Scene scene;
	CameraPage camera;
	Texture texture;

	float aspect;

	InputKeyField keyField{};
	InputDragField dragField{};

	DisplayCursor cursor{};

	float fov = 45.0f;

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
		GeometryPageCreateInfo createInfo{};
		createInfo.access = PAGE_ACCESS_RANDOM;
		createInfo.vertexCap = 2u << 10;
		createInfo.IndexCap = 2u << 12;
		createInfo.modelCap = 20u;

		if (createGeometryPage(renderDevice, &createInfo, &geometry))
			goto cleanup_6;
	}

	{
		SceneCreateInfo createInfo{};
		createInfo.geometry = geometry;
		createInfo.instanceCount = 4u;
		createInfo.drawCount = 10u;

		if (createScene(renderDevice, &createInfo, &scratch, &scene))
			goto cleanup_7;
	}

	{
		CameraCreateInfo createInfo{};
		createInfo.bindings = 2u;

		if (createCameraPage(renderDevice, &createInfo, &scratch, &camera))
			goto cleanup_8;
	}

	{
		TextureCreateInfo createInfo{};
		createInfo.path = "assets/textures/seaside.png";

		if (createTexture(renderDevice, &createInfo, &scratch, &texture))
			goto cleanup_9;
	}

	{
		const Vertex vertexData[] = {
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } },
			{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
			{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f } },
			{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f } },
		};

		const Index indexData[] = {
			0, 1, 2, 2, 3, 0
		};

		ModelAppendInfo model[1]{};
		model[0].vertexCount = 4u;
		model[0].pVertex = vertexData;
		model[0].indexCount = 6u;
		model[0].pIndex = indexData;

		GeometryWriteInfo write{};
		write.appendCount = 1u;
		write.pAppendInfo = model;

		if (writeGeometryPage(renderDevice, geometry, &write))
			goto cleanup_10;

		scratch.restore();
	}

	{
		InstanceData data[1];
		data[0] = { glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) };

		ObjectInstance instance{};
		instance.model = 0u;
		instance.instanceCount = 1u;
		instance.pInstances = data;

		if (pushObjectInstance(renderDevice, scene, &instance))
			goto cleanup_10;
	}

	{
		DrawInfo info[1]{};
		info[0].geometry = geometry;
		info[0].scene = scene;
		info[0].texture = texture;
		info[0].cameraIndex = 0u;

		if (recordDrawSequence(renderDevice, camera, 1u, info, SEQUENCE_RECORD_STATIC))
			goto cleanup_10;
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

		if (events & WINDOW_STATE_POINTER_MOTION_BIT)
			events &= ~WINDOW_STATE_POINTER_MOTION_BIT;
			
		pollInputs(inputDevice, &dragField, &keyField);

		if (isKeyPressed(&keyField, INPUT_KEY_R)) {
			dragField.delta[0] = 0u;
			dragField.delta[1] = 0u;
		}

		if (isKeyDown(&keyField, INPUT_KEY_ARROW_UP))
			fov -= 0.1;

		if (isKeyDown(&keyField, INPUT_KEY_ARROW_DOWN))
			fov += 0.1;

		{
			CameraData data[2]{};
			data[0].view = glm::lookAt(glm::vec3((float)dragField.delta[0] * fov / 5000.0f, -(float)dragField.delta[1] * fov / 5000.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			data[0].projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
			data[0].projection[1][1] *= -1;

			data[1].view = glm::lookAt(glm::vec3((float)dragField.delta[0] * fov / 5000.0f, -(float)dragField.delta[1] * fov / 5000.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			data[1].projection = glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, 0.1f, 100.0f);
			data[1].projection[1][1] *= -1;

			CameraWrite write{};
			write.firstCamera = 0u;
			write.count = 2u;
			write.pData = data;

			updateCamera(renderDevice, camera, &write);
		}

		switch (pushFrame(renderDevice)) {
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
	destroyGeometryPage(renderDevice, geometry);

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