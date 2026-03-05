#pragma once

#include "Signboard/RHI/rhi.h"
#include "Signboard/Platform/platform.h"

struct rndr_context_Access;

class rndr_context {
public:
	rndr_context() = default;

	rndr_context(const rndr_context&) = delete;
	rndr_context& operator=(const rndr_context&) = delete;

	void create(const platform::primitive::display_window& window);

	void rebuild_swapchain(const platform::primitive::display_window& window);

private:

	VkResult setup_instance() noexcept;
	VkResult setup_surface(const platform::primitive::display_window& window) noexcept;

	VkResult setup_device() noexcept;

	VkResult setup_allocator() noexcept;

private:
	friend struct rndr_context_Access;

	rhi::core::instance m_instance{};
	rhi::core::surface m_surface{};

	rhi::core::device m_device{};

	rhi::core::allocator m_allocator{};

};