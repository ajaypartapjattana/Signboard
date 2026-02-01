#pragma once

#include <cstdint>

namespace platform::core {

	class context {
	public:
		context();

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		context(context&&) = delete;
		context& operator=(context&&) = delete;

		~context();

		static const char** native_extensions(uint32_t& count);

	};

}