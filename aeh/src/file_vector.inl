#include "debug/assert.hh"

namespace aeh
{

	template <typename T, is_filesystem_trait FilesystemTrait, is_load_trait<T> LoadTrait>
	auto FileVector<T, FilesystemTrait, LoadTrait>::load(FilesystemTrait filesystem_trait_, LoadTrait load_trait_) -> FileVector
	{
		FileVector v = FileVector(std::move(filesystem_trait_), std::move(load_trait_));
		for (std::filesystem::path const & filename : v.filesystem_trait.files())
		{
			auto file = v.filesystem_trait.open_to_read(filename);
			debug_assert(static_cast<bool>(file));
			std::optional<T> content = v.load_trait.load(*file);
			debug_assert(content.has_value());
			v.elements.push_back(std::move(*content));
			v.filenames.push_back(filename);
		}
		return v;
	}

	template <typename T, is_filesystem_trait FilesystemTrait, is_load_trait<T> LoadTrait>
	auto FileVector<T, FilesystemTrait, LoadTrait>::add(T object, std::filesystem::path filename) -> bool
	{
		auto file = filesystem_trait.open_to_write(filename);
		if (!file)
			return false;

		bool const saved = load_trait.save(*file, object);
		if (saved)
		{
			elements.push_back(std::move(object));
			filenames.push_back(std::move(filename));
		}

		return saved;
	}

	template <typename T, is_filesystem_trait FilesystemTrait, is_load_trait<T> LoadTrait>
	auto FileVector<T, FilesystemTrait, LoadTrait>::remove(int index) -> bool
	{
		debug_assert(index >= 0 && index <= elements.size());
		bool const removed = filesystem_trait.remove(filenames[index]);
		if (removed)
		{
			elements.erase(elements.begin() + index);
			filenames.erase(filenames.begin() + index);
		}
		return removed;
	}

	template <typename T, is_filesystem_trait FilesystemTrait, is_load_trait<T> LoadTrait>
	auto FileVector<T, FilesystemTrait, LoadTrait>::replace(int index, T new_value) -> bool
	{
		debug_assert(index >= 0 && index <= elements.size());

		// If replacing with a value that is identical to what is already in the file, skip the operation.
		if constexpr (std::equality_comparable<T>)
			if (new_value == elements[index])
				return true;

		auto file = filesystem_trait.open_to_write(filenames[index]);
		if (!file)
			return false;

		bool const saved = load_trait.save(*file, new_value);
		if (saved)
			elements[index] = std::move(new_value);

		return saved;
	}

	template <function_ptr<auto() -> std::filesystem::path> get_base_path>
	auto ConstantBasePathTrait<get_base_path>::open_to_read(std::filesystem::path const & filename) const -> std::optional<std::ifstream>
	{
		auto file = std::ifstream(base_path() / filename, std::ios::binary);
		if (file.is_open())
			return file;
		else
			return std::nullopt;
	}

	template <function_ptr<auto() -> std::filesystem::path> get_base_path>
	auto ConstantBasePathTrait<get_base_path>::open_to_write(std::filesystem::path const & filename) const -> std::optional<std::ofstream>
	{
		auto file = std::ofstream(base_path() / filename, std::ios::binary);
		if (file.is_open())
			return file;
		else
			return std::nullopt;
	}

} // namespace aeh
