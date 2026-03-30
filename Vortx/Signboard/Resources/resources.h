#pragma once

#include "core/rsrc_core.h"
#include "procedure/rsrc_procedure.h"

class rndr_context;
class rndr_methods;

class Resources {
public:
	Resources(const rndr_context& context, const rndr_methods& methods) noexcept;

	void createVertexBuffer( size_t size);

	storage::vault_readAccessor<storage::storage_handle> read_vertBuffers() const noexcept;

private:
	void createDefaultVertBuffer();

private:
	rsrc_buffers m_buffers;
	rsrc_images m_images;

	rsrc_uploader m_uploader;

	storage::vault<storage::storage_handle> m_vertBuffers;
	storage::vault_writeAccessor<storage::storage_handle> m_vertWriter;

};