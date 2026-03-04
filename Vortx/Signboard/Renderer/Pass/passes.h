#pragma once

#include "Signboard/Core/Containers/storage/storage.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;

class passes {
public:
	passes(const rndr_context& context);

private:
	void setup_basePass();

private:
	const rhi::core::device& r_device;

	storage::vault<rhi::primitive::renderPass> m_passes;

};