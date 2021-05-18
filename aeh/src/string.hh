#pragma once

#include "compatibility.hh"
#include <string>
#include <filesystem>
#include <optional>

namespace aeh
{
	template <typename T> auto to_string(T const & t) noexcept -> decltype(std::to_string(t));
	constexpr auto to_string(std::string const & s) noexcept -> std::string const &;
	inline auto to_string(char const * s) noexcept -> std::string;
	inline auto to_string(std::string_view s) noexcept -> std::string;
	inline auto to_string(bool b) noexcept -> std::string;
	inline auto to_string(char c) noexcept -> std::string;
	auto to_string(std::filesystem::path const & p) noexcept -> std::string;
	
	template <typename ... Ts>
	auto join(Ts const & ... ts) noexcept -> std::string;

	auto is_null_terminated(std::string_view str) noexcept -> bool;
	auto make_null_terminated(std::string_view str) noexcept -> std::string_view;

	auto load_whole_file(std::string_view path) noexcept -> std::optional<std::string>;
	auto load_whole_file(std::filesystem::path const & path) noexcept -> std::optional<std::string>;
	auto replace(std::string_view string, std::string_view old_substr, std::string_view new_substr) noexcept -> std::string;

	auto make_string_view(char const first[], char const last[]) noexcept -> std::string_view;
	auto begin_ptr(std::string_view sv) noexcept -> char const *;
	auto end_ptr(std::string_view sv) noexcept -> char const *;

	auto count_lines(std::string_view text) noexcept -> int;
	auto split_first_line(std::string_view text) noexcept -> std::pair<std::string_view, std::string_view>;
	auto extract_first_line(std::string_view & text) noexcept -> std::string_view;
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

#if AEH_WINDOWS
	//! Opens a windows path selection dialog window and returns the selected paths.
	std::vector<std::string> browse();
	//! Will return empty string on failure to select a path or if multiple paths are selected.
	std::string browse_single_file();
	//! Dumps windows output directly to buffer. Used by the other browse functions.
	//! Returns true if at least one file has been selected.
	bool browse(char buffer[], size_t size) noexcept;
#endif

	namespace detail
	{
		template <typename T>
		concept has_size = requires(T const t) { {t.size()} -> std::convertible_to<size_t>; };
	}

	template <typename StringView, typename Delimiter>
	struct SplitIterator
	{
		struct Sentinel {};

		explicit SplitIterator(StringView text_, Delimiter delimiter_) noexcept : text(text_), delimiter(delimiter_)
		{
			next = text.find(delimiter);
			if (next == StringView::npos)
				next = text.size();
		}

		StringView operator * () const noexcept { return text.substr(index, next - index); }

		SplitIterator & advance() noexcept
		{
			if constexpr (detail::has_size<Delimiter>)
				index = next + delimiter.size();
			else
				index = next + 1;

			next = text.find(delimiter, index);
			if (next == StringView::npos)
				next = text.size();
			return *this;
		}

		SplitIterator & operator ++ () noexcept
		{
			return advance();
		}

		SplitIterator operator ++ (int)
		{
			auto copy = *this;
			advance();
			return copy;
		}

		friend bool operator == (SplitIterator const & split, Sentinel) noexcept { return split.index > split.text.size(); }

		SplitIterator begin() const noexcept { return *this; }
		Sentinel end() const noexcept { return Sentinel(); }

		StringView text;
		Delimiter delimiter;
		size_t index = 0;
		size_t next;
	};

	inline SplitIterator<std::string_view, char> split(std::string_view text, char delimiter) noexcept
	{
		return SplitIterator<std::string_view, char>(text, delimiter);
	}

	inline SplitIterator<std::string_view, std::string_view> split(std::string_view text, std::string_view delimiter) noexcept
	{
		return SplitIterator<std::string_view, std::string_view>(text, delimiter);
	}

	inline SplitIterator<std::u8string_view, std::u8string_view> split(std::u8string_view text, std::u8string_view delimiter) noexcept
	{
		return SplitIterator<std::u8string_view, std::u8string_view>(text, delimiter);
	}

	struct UTF8CodePointIterator
	{
		struct Sentinel {};

		explicit UTF8CodePointIterator(std::u8string_view text_) noexcept : text(text_)
		{
			advance();
		}

		unsigned const & operator * () const noexcept { return code_point; }

		UTF8CodePointIterator & advance() noexcept;

		UTF8CodePointIterator & operator ++ () noexcept
		{
			return advance();
		}

		UTF8CodePointIterator operator ++ (int)
		{
			auto copy = *this;
			advance();
			return copy;
		}

		friend bool operator == (UTF8CodePointIterator const & utf8_string, Sentinel) noexcept { return utf8_string.ended; }

		UTF8CodePointIterator begin() const noexcept { return *this; }
		Sentinel end() const noexcept { return Sentinel(); }

		std::u8string_view text;
		size_t index = 0;
		unsigned code_point;
		bool ended = false;
	};

	inline UTF8CodePointIterator view_as_utf8(std::u8string_view text) noexcept
	{
		return UTF8CodePointIterator(text);
	}

} // namespace aeh

#include "string.inl"
