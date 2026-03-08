#pragma once

#include "Signboard/Core/Containers/storage/storage.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;

class passes {
public:
	passes(const rndr_context& context);

	const rhi::primitive::renderPass& get_basePass() const noexcept;

private:
	void setup_basePass();

private:
	const rhi::core::device& r_device;

	rhi::primitive::renderPass m_basePass;
	storage::vault<rhi::primitive::renderPass> m_passes;

};