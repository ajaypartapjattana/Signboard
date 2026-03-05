#include "rndr_context.h"

void rndr_context::create(const platform::primitive::display_window& window) {
	VK_CHECK_VOID(setup_instance());
	VK_CHECK_VOID(setup_surface(window));
	VK_CHECK_VOID(setup_device());
	VK_CHECK_VOID(setup_allocator());
}

VkResult rndr_context::setup_instance() noexcept {
	rhi::procedure::instance_builder prcdr;

	uint32_t ext_count = 0;
	const char** exts = platform::core::context::native_extensions(ext_count);

	for (uint32_t i = 0; i < ext_count; ++i) {
		prcdr.addExtension(exts[i]);
	}

#ifndef NDEBUG
	prcdr.enableValidationLayer();
	prcdr.addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return prcdr.build(m_instance);
}

VkResult rndr_context::setup_surface(const platform::primitive::display_window& window) noexcept {
	rhi::procedure::surface_creator prcdr{ m_instance };

	return prcdr.create_surface(window, m_surface);
}

VkResult rndr_context::setup_device() noexcept {
	rhi::procedure::device_builder prcdr{ m_instance };

	prcdr.require_graphicsQueue();
	prcdr.require_transferQueue();
	prcdr.require_presentQueue(m_surface);

	prcdr.enable_samplerAnisotropy();

	return prcdr.build(m_device);
}

VkResult rndr_context::setup_allocator() noexcept {
	rhi::procedure::allocator_creator prcdr{ m_instance, m_device };

	return prcdr.create(m_allocator);
}