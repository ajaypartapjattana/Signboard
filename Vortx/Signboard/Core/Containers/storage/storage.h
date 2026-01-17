#pragma once

#include "storage_vault.h"
#include "storage_vault_accessor.h"

#include "storage_library.h"
#include "storage_library_accessor.h"

namespace storage {
	template <typename T> class Vault;
	template <typename T> class Library;

	template <typename T> class VaultReadAccessor;
	template <typename T> class VaultWriteAccessor;
	
	template <typename T> class LibraryReadAccessor;
	template <typename T> class LibraryWriteAccessor;
	
	namespace detail {
		template <typename Storage, typename T> class StorageReadIterator;
	}

}

