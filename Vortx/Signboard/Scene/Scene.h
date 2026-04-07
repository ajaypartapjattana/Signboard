#pragma once

#include "Signboard/Core/Containers/storage.h"
#include "Signboard/Assets/io/io.h"

class Resources;

struct scnObj {
	uint32_t mesh;
};

struct scnView {
	ctnr::vltView<scnObj> objView;
};

class Scene {
public:
	Scene();

	_NODISCARD scnView read_scene() const noexcept;	
	
	struct sceneObjectCraeteInfo {
		const char* modelPath;
	};
	void createSceneObject(const sceneObjectCraeteInfo& createInfo);

private:
	ctnr::vault<scnObj> m_objects;

};