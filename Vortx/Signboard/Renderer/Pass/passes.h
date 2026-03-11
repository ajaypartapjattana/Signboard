#pragma once

#include "Signboard/Core/Containers/storage/storage.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;
class rndr_presentation;

struct passes_Access;

class passes {
public:
	passes(const rndr_context& context, const rndr_presentation& presentation);

private:
	void setup_basePass();

private:
	friend struct passes_Access;

	const rhi::core::device& r_device;
	const rhi::primitive::swapchain& r_swapchain;

	rhi::primitive::renderPass m_primiaryPass;
	storage::vault<rhi::primitive::renderPass> m_passes;

};