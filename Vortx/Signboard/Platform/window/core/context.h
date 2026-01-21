#pragma once

namespace window::core {

	class context {
	public:
		context();

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		context(context&&) = delete;
		context& operator=(context&&) = delete;

		~context();

	};

}