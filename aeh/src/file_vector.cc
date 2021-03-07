#include "file_vector.hh"
#include <filesystem>

namespace aeh
{

	auto all_files_in(char const directory_path[]) -> std::vector<std::string>
	{
		std::vector<std::string> files;
		for (auto const & file : std::filesystem::directory_iterator(directory_path))
			files.push_back(file.path().string());
		return files;
	}

} // namespace aeh
