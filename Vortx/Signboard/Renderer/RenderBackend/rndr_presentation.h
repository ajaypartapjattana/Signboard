#pragma once

#include <vector>
#include "Signboard/Core/Containers/storage.h"
#include "Signboard/RHI/rhi.h"

class RHIContext;
class Resources;

struct rndr_presentation_Access;

class rndr_presentation {
public:
	rndr_presentation(const RHIContext& context, Resources& resources, const uint32_t bufferedFrame_count = 2);

	rndr_presentation(const rndr_presentation&) = delete;
	rndr_presentation& operator=(const rndr_presentation&) = delete;

	void recreate_swapchain(uint32_t* imageCount);
	ctnr::span<const uint32_t> expose_swapchainImages() const noexcept;

private:
	VkResult construct_swapchain();

private:
	friend struct rndr_presentation_Access;

	const rhi::creDevice& r_device;
	const rhi::creSurface& r_surface;
	Resources& r_resources;

	uint32_t bufferedFrame_count;

	rhi::pcdWatchdog m_watchdog;
	rhi::pcdSwapchainCreate m_swapchainCreate;
	
	rhi::pmvSwapchain m_swapchain;
	std::vector<uint32_t> swapchainImageHandles;


};