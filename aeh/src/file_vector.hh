#pragma once

#include "function_ptr.hh"
#include <string>
#include <vector>
#include <span>
#include <optional>

namespace aeh
{

	template <typename T>
	struct File
	{
		std::string path;
		T content;
	};

	template <typename R, typename T>
	concept range_of = std::ranges::range<R> && std::same_as<T, std::ranges::range_value_t<R>>;

	template <typename T>
	concept is_base_path_trait = std::movable<T> && requires (T const t) { 
		{t.base_path()} -> std::same_as<std::string>;
		{t.files_in_base_path()} -> range_of<std::string>;
	};

	template <typename Trait, typename T>
	concept is_load_trait = requires (Trait const trait, T const t, char const * const path) { 
		{trait.load(path)} -> std::same_as<std::optional<T>>;
		{trait.save(path, t)} -> std::same_as<bool>;
		{trait.remove(path)} -> std::same_as<bool>;
	};

	template <typename T, is_base_path_trait BasePathTrait, is_load_trait<T> LoadTrait>
	struct FileVector
	{
		static auto load(BasePathTrait base_path_trait_ = {}, LoadTrait load_trait_ = {})->FileVector;

		FileVector(FileVector const &) = delete;
		FileVector operator = (FileVector const &) = delete;
		FileVector(FileVector &&) noexcept = default;
		FileVector & operator = (FileVector &&) noexcept = default;

		[[nodiscard]] auto files() const noexcept -> std::span<File<T> const> { return all_files; }
		operator std::span<File<T> const>() const noexcept { return all_files; }

		auto add(T object, std::string_view filename) -> bool;
		auto remove(int index) -> bool;
		auto replace(int index, T new_value) -> bool;

	private:
		FileVector(BasePathTrait base_path_trait_, LoadTrait load_trait_) noexcept 
			: base_path_trait(std::move(base_path_trait_)) 
			, load_trait(load_trait_)
		{}

		std::vector<File<T>> all_files;
		[[no_unique_address]] BasePathTrait base_path_trait;
		[[no_unique_address]] LoadTrait load_trait;
	};

	[[nodiscard]] auto all_files_in(char const directory_path[]) -> std::vector<std::string>;

	template <function_ptr<auto() -> std::string> get_base_path>
	struct ConstantBasePathTrait
	{
		auto base_path() const -> std::string { return get_base_path(); }
		auto files_in_base_path() const -> std::vector<std::string> { return all_files_in(base_path().c_str()); }
	};

	struct VariableBasePathTrait
	{
		VariableBasePathTrait(std::string base_path_) noexcept : path(std::move(base_path_)) {}

		auto base_path() const -> std::string { return path; }
		auto files_in_base_path() const -> std::vector<std::string> { return all_files_in(base_path().c_str()); }

	private:
		std::string path;
	};

} // namespace aeh

#include "file_vector.inl"
