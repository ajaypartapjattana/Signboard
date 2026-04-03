#pragma once

#include <string>
#include <vector>
#include <memory>
#include <atomic>

#include "Signboard/Core/Threading/threadCommProtocol.h"
#include "Signboard/Core/Interfaces/uni_mesh/model.h"

namespace io {

	class ModelLoader {
	public:
		ModelLoader();

		ModelLoader(const ModelLoader&) = delete;
		ModelLoader& operator=(const ModelLoader&) = delete;

		~ModelLoader();

		void requestLoad(const std::string& objPath);
		bool hasPendingLoads() const;

		std::unique_ptr<Model> createCube(float sideLength);

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

}