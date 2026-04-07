#include "Scene.h"

#include "Signboard/Resources/resources.h"
#include "Signboard/Assets/io/io.h"

Scene::Scene()
{

}

scnView Scene::read_scene() const noexcept {
	return { ctnr::vltView<scnObj>{ m_objects } };
}

void Scene::createSceneObject(const sceneObjectCraeteInfo& createInfo) {
	
}
