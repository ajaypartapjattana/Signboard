#include "Scene.h"

#include "Signboard/Resources/resources.h"
#include "Signboard/Assets/io/io.h"

Scene::Scene(Resources& resources)
	:
	r_resources(resources)
{
	createDefaultCube();
}

void Scene::createDefaultCube() {
	auto _cube = m_modelLoader.createCube(0.5);
	
	io::loader::ModelLoader::Model& r_cube = *_cube.get();

	


}


scnView Scene::read_scene() const noexcept {
	return { ctnr::vltView<scnObj>{ m_objects } };
}

void Scene::createSceneObject(const sceneObjectCraeteInfo& createInfo) {
	
}
