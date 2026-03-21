#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Platform/platform.h"

struct rndr_context_Access;

class rndr_context {
public:
	rndr_context(const platform::primitive::displayWindow& window);

	rndr_context(const rndr_context&) = delete;
	rndr_context& operator=(const rndr_context&) = delete;

private:
	rhi::core::instance::createInfo get_instanceCreateInfo() noexcept;
	rhi::core::device::createInfo get_deviceCreateInfo() noexcept;

private:
	friend struct rndr_context_Access;

	rhi::core::instance m_instance;
	rhi::core::surface m_surface;

	rhi::core::device m_device;

	rhi::core::allocator m_allocator;

};