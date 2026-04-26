#pragma once

#include "resources.h"

struct ResourceView {
	ctnr::vltView<rhi::pmvBuffer> buffers;
	ctnr::vltView<rhi::pmvImage> images;

	ctnr::vltView<Mesh> meshes;
};

struct Resources_pAccess {

	static ResourceView create_view(const Resources& r) noexcept {
		return {
			ctnr::vltView<rhi::pmvBuffer>{ r.m_buffers },
			ctnr::vltView<rhi::pmvImage>{ r.m_images },
			ctnr::vltView<Mesh>{ r.m_meshes }
		};
	}

	static ctnr::vltWrite<rhi::pmvBuffer> get_bufferWrite(Resources& res) noexcept {
		return ctnr::vltWrite<rhi::pmvBuffer>{ res.m_buffers };
	}

	static ctnr::vltWrite<rhi::pmvImage> get_imageWrite(Resources& res) noexcept {
		return ctnr::vltWrite<rhi::pmvImage>{ res.m_images };
	}

};