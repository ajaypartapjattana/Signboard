#pragma once

#include "Signboard/Renderer/rndr.h"

namespace rndr {

	struct _pAccess {

		static inline const rhi::device& device(const context& context) noexcept { return context.device; }
		static inline const rhi::allocator& allocator(const context& context) noexcept { return context.allocator; }
		static inline const rhi::surface& surface(const context& context) noexcept { return context.surface; }

		static inline const resourceView view(const resources& resource) noexcept { return { resource.m_buffers, resource.m_images, resource.m_meshes }; }
		static inline sgb::vltWrite<rhi::pmvBuffer> bufferWrite(resources& resources) noexcept { return resources.m_buffers; }
		static inline sgb::vltWrite<rhi::pmvImage> imageWrite(resources& resources) noexcept { return resources.m_images; }

		static inline const rhi::swapchain& swapchain(const presentation& presentation) noexcept { return presentation.m_swapchain; }
		static inline const std::vector<uint32_t>& imageHandles(const presentation& presentation) noexcept { return presentation.swapchainImageHandles; }

		static inline const methodView view(const renderConfig& methods) noexcept { return { methods.m_renderPasses, methods.m_framebuffers, methods.m_pipelines }; }
		static inline sgb::vltQuery<renderTarget> targets(const renderConfig& method) noexcept { return method.m_renderTargets; }

	};

}

