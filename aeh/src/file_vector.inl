#include "debug/assert.hh"
#include <filesystem>

namespace aeh
{

	template <typename T, is_base_path_trait BasePathTrait, is_load_trait<T> LoadTrait>
	auto FileVector<T, BasePathTrait, LoadTrait>::load(BasePathTrait base_path_trait_, LoadTrait load_trait_) -> FileVector
	{
		FileVector v = FileVector(std::move(base_path_trait_), std::move(load_trait_));
		for (std::string path : v.base_path_trait.files_in_base_path())
		{
			std::optional<T> content = v.load_trait.load(path);
			debug_assert(content.has_value());
			v.all_files.push_back({std::move(path), std::move(*content)});
		}
		return v;
	}

	template <typename T, is_base_path_trait BasePathTrait, is_load_trait<T> LoadTrait>
	auto FileVector<T, BasePathTrait, LoadTrait>::add(T object, std::string_view filename) -> bool
	{
		std::string full_path = base_path_trait.base_path();
		full_path += filename;

		bool const saved = load_trait.save(full_path, object);
		if (saved)
			all_files.push_back({std::move(full_path), std::move(object)});
		return saved;
	}

	template <typename T, is_base_path_trait BasePathTrait, is_load_trait<T> LoadTrait>
	auto FileVector<T, BasePathTrait, LoadTrait>::remove(int index) -> bool
	{
		debug_assert(index >= 0 && index <= all_files.size());
		bool const removed = load_trait.remove(all_files[index].path);
		if (removed)
			all_files.erase(all_files.begin() + index);
		return removed;
	}

	template <typename T, is_base_path_trait BasePathTrait, is_load_trait<T> LoadTrait>
	auto FileVector<T, BasePathTrait, LoadTrait>::replace(int index, T new_value) -> bool
	{
		debug_assert(index >= 0 && index <= all_files.size());

		// If replacing with a value that is identical to what is already in the file, skip the operation.
		if constexpr (std::equality_comparable<T>)
			if (new_value == all_files[index].content)
				return true;

		bool const saved = load_trait.save(all_files[index].path, new_value);
		if (saved)
			all_files[index].content = std::move(new_value);
		return saved;
	}

} // namespace aeh
