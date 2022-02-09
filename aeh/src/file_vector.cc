#include "file_vector.hh"
#include <filesystem>

namespace aeh
{

	auto all_files_in(std::filesystem::path const & directory_path) -> std::vector<std::filesystem::path>
	{
		std::vector<std::filesystem::path> files;
		for (auto const & file : std::filesystem::directory_iterator(directory_path))
			files.push_back(file.path().filename());
		return files;
	}

	auto VariableBasePathTrait::remove(std::filesystem::path const & filename) const -> bool
	{
		return std::filesystem::remove(path / filename);
	}

	auto VariableBasePathTrait::open_to_read(std::filesystem::path const & filename) const -> std::optional<std::ifstream>
	{
		auto file = std::ifstream(path / filename, std::ios::binary);
		if (file.is_open())
			return file;
		else
			return std::nullopt;
	}

	auto VariableBasePathTrait::open_to_write(std::filesystem::path const & filename) const -> std::optional<std::ofstream>
	{
		auto file = std::ofstream(path / filename, std::ios::binary);
		if (file.is_open())
			return file;
		else
			return std::nullopt;
	}

} // namespace aeh
