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
	rhi::creInstance::createInfo get_instanceCreateInfo() noexcept;
	rhi::creDevice::createInfo get_deviceCreateInfo() noexcept;

private:
	friend struct rndr_context_Access;

	rhi::creInstance m_instance;
	rhi::creSurface m_surface;

	rhi::creDevice m_device;

	rhi::creAllocator m_allocator;

};