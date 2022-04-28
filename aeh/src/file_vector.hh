#pragma once

#include "function_ptr.hh"
#include <string>
#include <vector>
#include <span>
#include <optional>
#include <concepts>
#include <filesystem>
#include <fstream>

namespace aeh
{

	template <typename R, typename T>
	concept range_of = std::ranges::range<R> && std::same_as<T, std::ranges::range_value_t<R>>;

	template <typename T, typename Base>
	concept reference_to_derived_from = std::is_lvalue_reference_v<T> && std::derived_from<std::remove_reference_t<T>, Base>;

	template <typename T, typename Base>
	concept optional_of_derived_from = requires(T t)
	{
		{static_cast<bool>(t)};
		{*t} -> reference_to_derived_from<Base>;
	};

	template <typename Trait>
	concept is_filesystem_trait = std::movable<Trait> && requires (Trait const trait, typename Trait::path_type const & filename) {
		std::regular<typename Trait::path_type>;
		{trait.files()} -> range_of<typename Trait::path_type>;
		{trait.remove(filename)} -> std::same_as<bool>;
		{trait.open_to_read(filename)} -> optional_of_derived_from<std::istream>;
		{trait.open_to_write(filename)} -> optional_of_derived_from<std::ostream>;
	};

	template <typename Trait, typename T>
	concept is_load_trait = requires (Trait const trait, T const t, std::istream & input_stream, std::ostream & output_stream) { 
		{trait.load(input_stream)} -> std::same_as<std::optional<T>>;
		{trait.save(output_stream, t)} -> std::same_as<bool>;
	};

	template <typename T, is_filesystem_trait FilesystemTrait, is_load_trait<T> LoadTrait>
	struct FileVector
	{
		using path_type = typename FilesystemTrait::path_type;

		static auto load(FilesystemTrait filesystem_trait_ = {}, LoadTrait load_trait_ = {}) -> FileVector;

		FileVector(FileVector const &) = delete;
		FileVector operator = (FileVector const &) = delete;
		FileVector(FileVector &&) noexcept = default;
		FileVector & operator = (FileVector &&) noexcept = default;

		[[nodiscard]] auto values() const noexcept -> std::span<T const> { return elements; }
		operator std::span<T const>() const noexcept { return elements; }
		[[nodiscard]] auto size() const noexcept -> size_t { return elements.size(); }
		[[nodiscard]] auto operator [] (size_t i) const noexcept -> T const & { return elements[i]; }
		[[nodiscard]] auto filename_at(size_t i) const noexcept -> path_type const & { return filenames[i]; }

		auto add(T object, path_type filename) -> bool;
		auto remove(int index) -> bool;
		auto replace(int index, T new_value) -> bool;

	private:
		FileVector(FilesystemTrait filesystem_trait_, LoadTrait load_trait_) noexcept
			: filesystem_trait(std::move(filesystem_trait_)) 
			, load_trait(load_trait_)
		{}

		std::vector<T> elements;
		std::vector<path_type> filenames;
		[[no_unique_address]] FilesystemTrait filesystem_trait;
		[[no_unique_address]] LoadTrait load_trait;
	};

	[[nodiscard]] auto all_files_in(const std::filesystem::path & directory_path) -> std::vector<std::filesystem::path>;

	template <function_ptr<auto() -> std::filesystem::path> get_base_path>
	struct ConstantBasePathTrait
	{
		using path_type = std::filesystem::path;

		auto base_path() const -> std::filesystem::path { return get_base_path(); }
		auto files() const -> std::vector<std::filesystem::path> { return all_files_in(base_path()); }
		auto remove(std::filesystem::path const & filename) const -> bool { return std::filesystem::remove(base_path() / filename);}
		auto open_to_read(std::filesystem::path const & filename) const -> std::optional<std::ifstream>;
		auto open_to_write(std::filesystem::path const & filename) const -> std::optional<std::ofstream>;
	};

	struct VariableBasePathTrait
	{
		using path_type = std::filesystem::path;

		VariableBasePathTrait(std::filesystem::path base_path_) noexcept : path(std::move(base_path_)) {}

		auto base_path() const -> std::filesystem::path { return path; }
		auto files() const -> std::vector<std::filesystem::path> { return all_files_in(path); }
		auto remove(std::filesystem::path const & filename) const -> bool;
		auto open_to_read(std::filesystem::path const & filename) const -> std::optional<std::ifstream>;
		auto open_to_write(std::filesystem::path const & filename) const -> std::optional<std::ofstream>;

	private:
		std::filesystem::path path;
	};

} // namespace aeh

#include "file_vector.inl"
