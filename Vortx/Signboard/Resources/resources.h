#pragma once

#include "core/rsrc_core.h"
#include "procedure/rsrc_procedure.h"

class rndr_context;
class rndr_methods;

class Resources {
public:
	Resources(const rndr_context& context, const rndr_methods& methods) noexcept;

	void createVertexBuffer( size_t size);
	ctnr::vltView_const<uint32_t> read_vertBuffers() const noexcept;

private:
	void createDefaultVertBuffer();

private:
	rsrc_buffers m_buffers;
	rsrc_images m_images;

	rsrc_uploader m_uploader;

	ctnr::vault<uint32_t> m_vertBuffers;
	ctnr::vault_writeAccessor<uint32_t> m_vertWriter;

};