#pragma once

#include "rndr_method.h"

struct rndr_method_Access {

	static methodView read_methods(const rndr_method& m) noexcept {

		return {
			m.m_passes.read_renderPasses(),
			m.m_passes.read_framebuffers(),
			m.m_materials.read_pipelines()
		};

	}

};