#pragma once

class rndr_context;
class rndr_presentation;
class rndr_method;

#include "Signboard/RHI/rhi.h"

class rndr_framedraw {
public:
	rndr_framedraw(const rndr_context& context, const rndr_presentation& presentation, const rndr_method& methods);

	rndr_framedraw(const rndr_framedraw&) = delete;
	rndr_framedraw& operator=(const rndr_framedraw&) = delete;

	void drawFrame(const uint32_t target_index, const rhi::primitive::commandBuffer& target_cmd);

private:
	VkResult create_primaryPass_framebuffers();

private:
	const rhi::core::device& r_device;
	const rhi::primitive::swapchain& r_swapchain;

	const rhi::primitive::renderPass& r_primaryPass;

	uint32_t bufferedFrame_count = 2;
	std::vector<rhi::primitive::framebuffer> m_primaryFramebuffers;


};