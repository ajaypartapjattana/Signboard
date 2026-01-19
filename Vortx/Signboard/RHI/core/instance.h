#pragma once

namespace rhi::procedure {
	class instance_builder;
}

namespace rhi::core {

	struct instance_vkAccess;

	class instance {
	public:
		instance(const instance&) = delete;
		instance& operator=(const instance&) = delete;

		instance(instance&&) noexcept;
		instance& operator=(instance&&) noexcept;

		~instance() noexcept;

		const void* native_instance() const noexcept;

	private:
		friend class rhi::procedure::instance_builder;
		friend struct instance_vkAccess;

		instance() = default;

		void* m_instance = nullptr;
		void* m_allocator = nullptr;

	};

}