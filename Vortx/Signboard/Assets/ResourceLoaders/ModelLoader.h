#pragma once

#include "Signboard/Core/Threading/threadCommProtocol.h"
#include "Signboard/Renderer/Mesh/Vertex.h"

#include <string>
#include <vector>
#include <memory>
#include <atomic>

struct Model {
	const std::string obj_FilePath;

	struct ModelAttributes {
		size_t trisCount;
		size_t vertCount;
		size_t uniqueVertCount;
	} attributes{ 0 ,0 ,0 };

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Model(const std::string& path)
		: obj_FilePath(path) {
	}

};

class ModelLoader {
public:
	ModelLoader();

	ModelLoader(const ModelLoader&) = delete;
	ModelLoader& operator=(const ModelLoader&) = delete;

	~ModelLoader();

	void requestLoad(const std::string& objPath);

	bool hasPendingLoads() const;
	std::vector<std::unique_ptr<Model>> collectLoadedModels();

private:
	void load(Model&);

	void loaderLoop();

private:
	std::atomic<bool> running{ false };
	std::atomic<uint32_t> outstandingLoads{ 0 };

	ThreadSafeQueue<std::string> loadRequests;
	ThreadSafeQueue<std::unique_ptr<Model>> loadedModels;

	std::thread loaderThread;

	std::mutex wakeMutex;
	std::condition_variable wakeCV;

};