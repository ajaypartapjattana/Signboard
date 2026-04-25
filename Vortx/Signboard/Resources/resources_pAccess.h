#pragma once

#include "resources.h"

struct Resources_pAccess {

	static ctnr::vault_writeAccessor<rhi::pmvBuffer> get_bufferWrite(Resources& res) noexcept {
		return ctnr::vault_writeAccessor<rhi::pmvBuffer>{ res.m_buffers };
	}

	static ctnr::vault_writeAccessor<rhi::pmvImage> get_imageWrite(Resources& res) noexcept {
		return ctnr::vault_writeAccessor<rhi::pmvImage>{ res.m_images };
	}

};