#include "file_loader.h"

#include <fstream>

namespace io::loader {

	file_loader::file_loader(char** tw_target, size_t* tw_size) noexcept
		: m_target(tw_target), m_targetSize(tw_size)
	{

	}

	bool file_loader::load_binary(const std::filesystem::path& path) {
		if (!m_target || !m_targetSize)
			return false;
		
		std::ifstream file{ path, std::ios::binary | std::ios::ate };

		if (!file)
			return false;

		const std::streamsize size = file.tellg();

		if (size <= 0)
			return false;
		
		char* newBuffer = new (std::nothrow) char[static_cast<size_t>(size)];
		if (!newBuffer)
			return false;


		file.seekg(0, std::ios::beg);
		if (!file.read(newBuffer, size)) {
			delete[] newBuffer;
			return false;
		}

		delete[] *m_target;

		*m_target = newBuffer;
		*m_targetSize = static_cast<size_t>(size);

		return true;
	}

}