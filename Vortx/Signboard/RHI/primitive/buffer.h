#pragma once

#include "Signboard/RHI/detail/vma/vma.h"

namespace rhi::access {
	struct buffer_pAccess;
}

namespace rhi {

	class pcdBufferAllocator;

	class pmvBuffer {
	public:
		pmvBuffer() noexcept;

		pmvBuffer(const pmvBuffer&) = delete;
		pmvBuffer& operator=(const pmvBuffer&) = delete;

		pmvBuffer(pmvBuffer&&) noexcept;
		pmvBuffer& operator=(pmvBuffer&&) noexcept;

		~pmvBuffer() noexcept;

		void* native_mapped() const;
		void flush(VkDeviceSize begin, VkDeviceSize end) const;

	private:
		friend class pcdBufferAllocator;
		friend struct rhi::access::buffer_pAccess;

		VkBuffer m_buffer;
		VmaAllocation allocation;

		bool _is_host_coherent;
		void* _mpd;

		VmaAllocator _allctr;

	};

}