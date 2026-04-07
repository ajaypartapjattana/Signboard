#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Platform/platform.h"

struct rndr_context_Access;

class RHIContext {
public:
	RHIContext(const platform::primitive::displayWindow& window);

	RHIContext(const RHIContext&) = delete;
	RHIContext& operator=(const RHIContext&) = delete;

private:
	rhi::creInstance::createInfo get_instanceCreateInfo() noexcept;
	rhi::creDevice::createInfo get_deviceCreateInfo() noexcept;

private:
	friend struct rndr_context_Access;

	rhi::creInstance m_instance;
	rhi::creSurface m_surface;

	rhi::creDevice _dvc;

	rhi::creAllocator _allctr;

};