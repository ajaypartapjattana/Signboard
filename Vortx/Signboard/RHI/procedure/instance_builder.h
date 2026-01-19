#pragma once

#include <vector>

namespace rhi::core {
	class instance;
}

namespace rhi::procedure {

	class instance_builder {
	public:
		instance_builder();

		instance_builder(const instance_builder&) = delete;
		instance_builder& operator=(const instance_builder&) = delete;

		instance_builder& addExtension(const char* name);
		instance_builder& enableValidationLayer();

		rhi::core::instance build();

	private:
		static bool extensionSupported(const char*);
		static bool layerSupported(const char*);

	private:
		std::vector<const char*> m_extensions;
		std::vector<const char*> m_layers;

	};

}