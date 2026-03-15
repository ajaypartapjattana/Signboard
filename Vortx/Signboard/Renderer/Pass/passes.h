#pragma once

#include "Signboard/Core/Containers/storage/storage.h"
#include "Signboard/RHI/rhi.h"

class rndr_context;
class rndr_presentation;

struct passes_Access;

class passes {
public:
	passes(const rhi::core::device& device, const rhi::primitive::swapchain& swapchain);

	struct createInfo {
		bool swapchain_pass = true;
		VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	};

	storage::storage_handle create_pass(const createInfo* info);
	storage::vault_readAccessor<rhi::primitive::renderPass> get_readAccessor() const noexcept;

private:
	friend struct passes_Access;

	const rhi::core::device& r_device;
	const rhi::primitive::swapchain& r_swapchain;

	storage::vault<rhi::primitive::renderPass> m_renderPasses;
	storage::vault_writeAccessor<rhi::primitive::renderPass> m_writeAccess;

};