#pragma once

#include "Signboard/RHI/rhi.h"

class rndr_context;

struct rndr_presentation_Access;

class rndr_presentation {
public:
	rndr_presentation(const rndr_context& context, const uint32_t bufferedFrame_count = 2);

	rndr_presentation(const rndr_presentation&) = delete;
	rndr_presentation& operator=(const rndr_presentation&) = delete;

	void recreate_swapchain(uint32_t* imageCount);
	void update_bufferedFrameCount(const uint32_t count);

private:
	VkResult construct_swapchain();

private:
	friend struct rndr_presentation_Access;

	const rhi::creDevice& r_device;
	const rhi::creSurface& r_surface;

	rhi::pcdWatchdog m_watchdog;

	uint32_t bufferedFrame_count;
	rhi::pmvSwapchain m_swapchain;

};