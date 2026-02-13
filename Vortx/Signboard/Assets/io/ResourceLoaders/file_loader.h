#pragma once

#include <filesystem>
#include <cstdint>

namespace io::loader {

	class file_loader {
	public:
		file_loader(char** target, size_t* size) noexcept;

		bool load_binary(const std::filesystem::path& path);

	private:
		char** m_target = nullptr;
		size_t* m_targetSize = nullptr;

	};

}