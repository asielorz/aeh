#pragma once

#include "compatibility.hh"
#include "generator.hh"
#include "debug/assert.hh"
#include <string>
#include <filesystem>
#include <span>
#include <string_view>

namespace aeh
{
	template <typename T> auto to_string(T const& t) noexcept -> decltype(std::to_string(t));
	constexpr auto to_string(std::string const& s) noexcept -> std::string const&;
	inline auto to_string(char const* s) noexcept -> std::string;
	inline auto to_string(std::string_view s) noexcept -> std::string;
	inline auto to_string(bool b) noexcept -> std::string;
	inline auto to_string(char c) noexcept -> std::string;
	auto to_string(std::filesystem::path const& p) noexcept -> std::string;

	template <typename ... Ts>
	auto join(Ts const & ... ts) noexcept -> std::string;

	auto is_null_terminated(std::string_view str) noexcept -> bool;
	auto make_null_terminated(std::string_view str) noexcept -> std::string_view;

	auto load_whole_file(std::string_view path) noexcept -> std::optional<std::string>;
	auto load_whole_file(std::filesystem::path const& path) noexcept -> std::optional<std::string>;
	auto replace(std::string_view string, std::string_view old_substr, std::string_view new_substr) noexcept -> std::string;
	void replace_in_place(std::string& string, std::string_view old_substr, std::string_view new_substr) noexcept;

	auto make_string_view(char const first[], char const last[]) noexcept -> std::string_view;
	auto begin_ptr(std::string_view sv) noexcept -> char const*;
	auto end_ptr(std::string_view sv) noexcept -> char const*;

	auto count_lines(std::string_view text) noexcept -> int;
	auto split_first_line(std::string_view text) noexcept -> std::pair<std::string_view, std::string_view>;
	auto extract_first_line(std::string_view& text) noexcept -> std::string_view;
	auto ignore_empty_lines(std::string_view text) noexcept -> std::string_view; // Removes all empty lines from the beginning.
	auto ignore_starting_whitespace(std::string_view text) noexcept -> std::string_view;
	auto ignore_trailing_whitespace(std::string_view text) noexcept -> std::string_view;
	auto ignore_whitespace(std::string_view text) noexcept -> std::string_view;
	auto starts_with(std::string_view string, std::string_view prefix) noexcept -> bool;
	auto is_contained_in(std::string_view string, std::string_view substr) noexcept -> bool;

	auto indent(int indentation_level) noexcept -> std::string;

	//! For strings representing a path, returns the path of the directory that contains that path
	std::string_view parent_path(std::string_view path) noexcept;
	//! For strings that represent a path, returns the whole filename, including extension.
	std::string_view filename(std::string_view path) noexcept;
	//! For strings that represent a path, returns the filename without the extension.
	std::string_view stem(std::string_view path) noexcept;
	//! For strings that represent a path, returns the extension without the extension.
	std::string_view extension(std::string_view path) noexcept;

	constexpr auto is_whitespace = [](char c) noexcept -> bool { return c == ' ' || c == '\t'; };
	constexpr auto is_whitespace_or_newline = [](char c) noexcept -> bool { return c == ' ' || c == '\t' || c == '\n'; };
	constexpr auto is_number = [](char c) noexcept -> bool { return (c >= '0' && c <= '9'); };
	constexpr auto is_letter = [](char c) noexcept -> bool { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); };

	struct AllowedFileType
	{
		std::u8string_view description;
		std::span<std::u8string_view const> extensions;

		static constexpr AllowedFileType all_files_allowed()
		{
			return { u8"All Files", all_extensions_allowed_wildcard };
		}

	private:
		static constexpr std::array<std::u8string_view, 1> all_extensions_allowed_wildcard = { u8"" };
	};

	constexpr std::array<AllowedFileType, 1> default_allowed_file_types = {AllowedFileType::all_files_allowed()};

	struct OpenSingleFileOptions
	{
		std::u8string_view title = u8"Open file";
		std::span<AllowedFileType const> allowed_file_types = default_allowed_file_types;
	};

	struct OpenMultipleFileOptions
	{
		std::u8string_view title = u8"Open files";
		std::span<AllowedFileType const> allowed_file_types = default_allowed_file_types;
	};

	auto open_single_file(OpenSingleFileOptions options = {}) -> std::optional<std::filesystem::path>;
	auto open_multiple_files(OpenMultipleFileOptions options = {}) -> std::vector<std::filesystem::path>;

	struct SaveFileOptions
	{
		std::u8string_view title = u8"Save file";
		std::filesystem::path default_path;
		bool file_overwrite_warning = true;
		std::span<AllowedFileType const> allowed_file_types = default_allowed_file_types;
	};

	auto save_file(SaveFileOptions options = {}) -> std::optional<std::filesystem::path>;

	struct OpenDirectoryOptions
	{
		std::u8string_view title = u8"Open folder";
		std::filesystem::path default_path;
	};

	auto open_directory(OpenDirectoryOptions options = {}) -> std::optional<std::filesystem::path>;

	//! Returns an aeh::generator object that generates the splitted string parts.
	constexpr auto split(std::string_view text, char delimiter) noexcept;
	constexpr auto split(std::string_view text, std::string_view delimiter) noexcept;
	constexpr auto split(std::u8string_view text, char8_t delimiter) noexcept;
	constexpr auto split(std::u8string_view text, std::u8string_view delimiter) noexcept;

	//! Returns an aeh::generator object that generates the UTF8 code points.
	inline auto view_as_utf8(std::u8string_view text) noexcept;

	//! Like strncpy but guarantees that the destination is null terminated,
	//! even when a truncation occurs. The extra space (if any) is not written to
	void c_string_copy(std::span<char> dest, char const src[]);
	void c_string_copy(std::span<char> dest, std::string_view src);

} // namespace aeh

#include "string.inl"
