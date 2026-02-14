#pragma once

#include "Signboard/Core/Containers/storage/storage.h"
#include "Signboard/RHI/rhi.h"

class render_interface;

class passes {
public:
	passes(const render_interface& ri);

private:
	void setup_basePass();

private:
	const rhi::core::device& m_device;

	storage::vault<rhi::primitive::renderPass> m_passes;

};