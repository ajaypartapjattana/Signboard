#pragma once

#include "Signboard/Core/Containers/storage/storage.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;
class rndr_interface;

class passes {
public:
	passes(const rndr_context& context, const rndr_interface& interface);

	const rhi::primitive::renderPass& get_basePass() const noexcept;

private:
	void setup_basePass();

private:
	const rhi::core::device& r_device;
	const rhi::primitive::swapchain& r_swapchain;

	rhi::primitive::renderPass m_basePass;
	storage::vault<rhi::primitive::renderPass> m_passes;

};