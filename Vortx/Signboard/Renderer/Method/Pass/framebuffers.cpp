#include "framebuffers.h"

#include <stdexcept>

namespace rndr {

	framebuffers::framebuffers(const rhi::device& device, sgb::vltWrite<rhi::pmvFramebuffer> framebufferWrite, const sgb::vltView<rhi::pmvImage>& images) noexcept
		:
		r_images(images),
		m_framebufferCreate(device),
		_wrt(framebufferWrite)
	{

	}

	uint32_t framebuffers::createFramebuffers(const rhi::pmvRenderPass& renderPass, sgb::span<const uint32_t> imageIndices) {
		if (imageIndices.empty()) {
			throw std::logic_error("FAILURE: invalid_input_size!");
		}
		
		size_t _iSz = imageIndices.size();

		std::vector<const rhi::pmvImage*> images;

		images.reserve(_iSz);
		for (size_t i = 0; i < _iSz; ++i) {
			images.emplace_back(r_images.get(imageIndices[i]));
		}

		m_framebufferCreate.target_attachments(images);
		
		m_framebufferCreate.target_renderPass(renderPass);

		auto _ctor = [&](rhi::pmvFramebuffer* fb) {
			VkResult result = m_framebufferCreate.publish(*fb);
			if (result == VK_SUCCESS)
				return;

			throw std::runtime_error("FAILURE: framebuffer_creation!");
		};

		return _wrt.construct(_ctor);
	}

}

