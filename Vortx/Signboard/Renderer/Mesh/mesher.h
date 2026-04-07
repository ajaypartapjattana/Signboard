#pragma once

#include <memory>

#include "Signboard/Core/Interfaces/renderer/model/model.h"

enum class MESH_PRIMITIVE {
	CUBE, ICOSPHERE, CYLINDER
};

class Mesher {
public:
	Mesher() = default;

	std::unique_ptr<Model> createPrimitive(const MESH_PRIMITIVE primitive, const float size);

private:

};