#include "commandPool.h"

namespace rhi {

	void commandPool::reset() noexcept {
		if (m_commandPool) {
			vkDestroyCommandPool(r_device, m_commandPool, nullptr);
		}

		m_commandPool = VK_NULL_HANDLE;
	}

}