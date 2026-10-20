#include <stdio.h>

#include <ECS/camera.h>

#include <core/memory.h>
#include <core/io/io.h>

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
	virmem_init();

	mem_stack scratch;

	if (scratch.create(32u << 20))
		return EXIT_FAILURE;

	WindowInput input;
	DisplayContext windowCtx;
	DisplayWindow window;

	VulkanContext vulkanCtx;
	RenderDevice renderDevice;

	GeometryPage geometry;
	Scene scene;
	CameraPage camera;
	Texture texture;

	PerspectiveCamera cam;

	{
		if (requestDisplayContext(&windowCtx))
			goto cleanup_0;

		{
			WindowCreateInfo createInfo{};
			createInfo.flags = WINDOW_CREATE_FULLSCREEN_BIT;
			createInfo.geometry = { 0, 0, 800, 800 };
			createInfo.title = "My Window";
			createInfo.pInput = &input;

			if (createDisplayWindow(windowCtx, &createInfo, &window))
				goto cleanup_1;
		}
	}

	{
		if (createVulkanContext(&scratch, &vulkanCtx))
			goto cleanup_2;

		uint32_t deviceCount;
		getPhysicalDeviceCount(vulkanCtx, &deviceCount);
		
		if (!deviceCount) {
			printf("FAILURE : no_vulkan_support.\n");
			goto cleanup_3;
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
			goto cleanup_3;
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
			goto cleanup_3;
	}

	raiseDisplayWindow(window);

	{
		GeometryPageCreateInfo createInfo{};
		createInfo.access = PAGE_ACCESS_RANDOM;
		createInfo.vertexCap = 2u << 10;
		createInfo.IndexCap = 2u << 12;
		createInfo.modelCap = 20u;

		if (createGeometryPage(renderDevice, &createInfo, &geometry))
			goto cleanup_4;
	}

	{
		SceneCreateInfo createInfo{};
		createInfo.geometry = geometry;
		createInfo.instanceCount = 4u;
		createInfo.drawCount = 10u;

		if (createScene(renderDevice, &createInfo, &scratch, &scene))
			goto cleanup_5;
	}

	{
		CameraCreateInfo createInfo{};
		createInfo.bindings = 2u;

		if (createCameraPage(renderDevice, &createInfo, &scratch, &camera))
			goto cleanup_6;
	}

	{
		TextureCreateInfo createInfo{};
		createInfo.path = "assets/textures/seaside.png";

		if (createTexture(renderDevice, &createInfo, &scratch, &texture))
			goto cleanup_7;
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
			goto cleanup_8;

		scratch.restore();
	}

	{
		InstanceData data[1] = { glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) };

		ObjectInstance instance{};
		instance.model = 0u;
		instance.instanceCount = 1u;
		instance.pInstances = data;

		if (pushObjectInstance(renderDevice, scene, &instance))
			goto cleanup_8;
	}

	{
		DrawInfo info[1]{};
		info[0].geometry = geometry;
		info[0].scene = scene;
		info[0].texture = texture;
		info[0].cameraIndex = 0u;

		if (recordDrawSequence(renderDevice, camera, 1u, info, SEQUENCE_RECORD_STATIC))
			goto cleanup_8;
	}

	cam.moveTo(glm::vec3(0.0f, 0.0f, 2.0f));

	while (true) {
		input.flush();
		pollWindowEvents();

		WindowNotification notification = queryWindowNotification(window);
		if (notification) {
			if (notification & WINDOW_NOTIFY_TERMINATION_BIT) {
				waitRenderDevice(renderDevice);
				break;
			}

			if (notification & WINDOW_NOTIFY_STATE_BIT) {
				switch (getWindowState(window)) {
				case WINDOW_STATE_UNDEFINED:
					waitRenderDevice(renderDevice);
					goto cleanup_8;

				case WINDOW_STATE_MINIMIZED:
					waitWindowEvents();
					break;

				case WINDOW_STATE_WINDOWED:
				case WINDOW_STATE_MAXIMIZED:
					if (configureRenderDevice(renderDevice)) {
						waitRenderDevice(renderDevice);
						goto cleanup_8;
					}

					DrawInfo info[1]{};
					info[0].geometry = geometry;
					info[0].scene = scene;
					info[0].texture = texture;
					info[0].cameraIndex = 0u;

					if (recordDrawSequence(renderDevice, camera, 1u, info, SEQUENCE_RECORD_STATIC)) {
						waitRenderDevice(renderDevice);
						goto cleanup_8;
					}

					WindowGeomentry rect;
					queryWindowGeometry(window, &rect);

					cam.aspect(rect.width / (float)rect.height);

					break;

				}
			}

			if (notification & WINDOW_NOTIFY_INPUT_BIT) {
				input.lapse();

				if (input.isPressed(KEY_ESCAPE)) {
					waitRenderDevice(renderDevice);
					goto cleanup_8;
				}

				if (input.isPressed(KEY_R)) {
					cam.moveTo(glm::vec3(0.0f, 0.0f, 2.0f));
					cam.lookAt(glm::vec3(0.0f));
				}
			}

			clearWindowNotification(window);
		}

		if (input.testDown()) {
			glm::vec3 movDir(0.0f);

			if (input.isDown(KEY_W))
				movDir.z -= 1.0f;

			if (input.isDown(KEY_S))
				movDir.z += 1.0f;

			if (input.isDown(KEY_A))
				movDir.x -= 1.0f;

			if (input.isDown(KEY_D))
				movDir.x += 1.0f;

			if (input.isDown(KEY_SHIFT_LEFT))
				movDir.y -= 1.0f;

			if (input.isDown(KEY_CTRL_LEFT))
				movDir.y += 1.0f;

			cam.moveUniformL(movDir, 0.05f);

			if (input.isDown(KEY_ARROW_UP))
				cam.rotateL(CAMERA_RT, -0.01f);

			if (input.isDown(KEY_ARROW_DOWN))
				cam.rotateL(CAMERA_RT, 0.01f);
			
			if (input.isDown(KEY_ARROW_RIGHT))
				cam.rotateL(CAMERA_UP, -0.01f);

			if (input.isDown(KEY_ARROW_LEFT))
				cam.rotateL(CAMERA_UP, 0.01f);
		}

		{
			IVEC2D pointer = input.pointer();

			CameraData data[1]{};
			data[0].view = cam.view();
			data[0].projection = cam.project();

			CameraWrite write{};
			write.firstCamera = 0u;
			write.count = 1u;
			write.pData = data;

			updateCamera(renderDevice, camera, &write);
		}

		switch (pushFrame(renderDevice)) {
		case 0:
			break;
		case 1:
			if (configureRenderDevice(renderDevice)) {
				waitRenderDevice(renderDevice);
				goto cleanup_8;
			}
			break;
		default:
			waitRenderDevice(renderDevice);
			goto cleanup_8;
		}
	}

cleanup_8:
	destroyTexture(renderDevice, texture);

cleanup_7:
	destroyCamera(renderDevice, camera);

cleanup_6:
	destroyScene(renderDevice, scene);

cleanup_5:
	destroyGeometryPage(renderDevice, geometry);

cleanup_4:
	destroyRenderDevice(vulkanCtx, renderDevice);

cleanup_3:
	destroyVulkanContext(vulkanCtx);

cleanup_2:
	destroyDisplayWindow(window);

cleanup_1:
	destroyDisplayContext(windowCtx);

cleanup_0:
	scratch.reset();

	return 0;
}
