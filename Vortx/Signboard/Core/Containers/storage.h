#pragma once

#include "storage/vault.h"
#include "storage/vault_accessor.h"

#include "storage/storage_library.h"
#include "storage/storage_library_accessor.h"

namespace storage {

	struct storage_handle;

	template <typename T> class vault;
	template <typename T> class vault_readAccessor;

	template <typename T> class library;
	template <typename T> class library_readAccessor;

	template <typename T> class vault_writeAccessor;
	template <typename T> class library_writeAccessor;
	
	namespace detail {
		template <typename Storage, typename T> class storage_readIterator;
	}

}

