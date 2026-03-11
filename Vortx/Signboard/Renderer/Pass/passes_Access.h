#pragma once

#include "passes.h"

struct passes_Access {

	static const rhi::primitive::renderPass& get_primiaryPass(const passes& p) noexcept {
		return p.m_primiaryPass;
	}

};