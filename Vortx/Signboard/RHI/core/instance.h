#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi {

	struct _pAccess;

	class creInstance {
	public:
		struct createInfo {
			std::vector<const char*> extensions;
			std::vector<const char*> layers;
			bool enable_validation = false;
		};

		creInstance(const createInfo& createInfo) noexcept;

		creInstance(const creInstance&) = delete;
		creInstance& operator=(const creInstance&) = delete;

		creInstance(creInstance&&) noexcept;
		creInstance& operator=(creInstance&&) noexcept;

		~creInstance() noexcept;

	private:
		void build(const createInfo& createInfo);

		bool validateExtension(const char* extName);
		bool validateLayer(const char* layerName);

	private:
		friend struct _pAccess;

		VkInstance m_instance;

	};

}