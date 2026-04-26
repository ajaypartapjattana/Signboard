#pragma once

#include "Signboard/RHI/rhi.h"

#include "rndr_method.h"
#include "Signboard/Scene/Scene.h"

struct ResourceView;

class rndr_framedraw {
public:
	rndr_framedraw(const rndr_method& methods, const ResourceView& resources);

	rndr_framedraw(const rndr_framedraw&) = delete;
	rndr_framedraw& operator=(const rndr_framedraw&) = delete;

	void drawFrame(const uint32_t target_index, const rhi::pmvCommandBuffer& target_cmd);

private:
	const ctnr::vltView<renderTarget> m_targets;
	methodView m_methodRead;

	const ResourceView& r_resources;

};