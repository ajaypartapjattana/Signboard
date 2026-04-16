#pragma once

#include <filesystem>
#include <cstdint>

namespace io::loader {

	class file_loader {
	public:
		std::vector<uint32_t> load_SPIRV(const std::filesystem::path& path);
		std::vector<char> load_BIN(const std::filesystem::path& path);

	};

}