#include "string.hh"
#include "debug/assert.hh"
#include "internal/portable_file_dialogs.hh"
#include <imgui.h> // Needed to include <imgui_internal.h>
#include <imgui_internal.h> // ImTextCharFromUtf8
#include <fstream>
#include <algorithm>

namespace aeh
{

	auto to_string(std::filesystem::path const & p) noexcept -> std::string
	{
		return p.string();
	}

	auto is_null_terminated(std::string_view str) noexcept -> bool
	{
		return str.data()[str.size()] == '\0';
	}

	auto make_null_terminated(std::string_view str) noexcept -> std::string_view
	{
		if (is_null_terminated(str))
			return str;
		else
		{
			char * const new_buffer = static_cast<char *>(alloca(str.size() + 1));
			std::copy(str.begin(), str.end(), new_buffer);
			new_buffer[str.size()] = '\0';
			return {new_buffer, str.size()};
		}
	}

	static auto load_whole_file_impl(std::ifstream& file) noexcept -> std::optional<std::string>
	{
		// The "naive" way of doing it, reading using std::istreambuf_iterator
		// is unnecessarily slow:
		// https://stackoverflow.com/a/2602060/11929731
		// https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
		if (!file.is_open())
			return std::nullopt;

		file.seekg(0, std::ios::end);

		std::string contents;
		contents.resize(file.tellg());

		file.seekg(0, std::ios::beg);
		file.read(&contents[0], contents.size());
		return contents;
	}

	auto load_whole_file(std::string_view path) noexcept -> std::optional<std::string>
	{
		std::ifstream file(make_null_terminated(path).data());
		return load_whole_file_impl(file);
	}

	auto load_whole_file(std::filesystem::path const & path) noexcept -> std::optional<std::string>
	{
		std::ifstream file(path);
		return load_whole_file_impl(file);
	}

	auto replace(std::string_view string, std::string_view old_substr, std::string_view new_substr) noexcept -> std::string
	{
		debug_assert(old_substr.size() > 0);

		std::string result;
		for (size_t i = 0; i < string.size();)
		{
			size_t const new_i = string.find(old_substr, i);
			if (new_i == std::string_view::npos)
			{
				result += string.substr(i, string.size() - i);
				break;
			}

			result += string.substr(i, new_i - i);
			result += new_substr;
			i = new_i + old_substr.size();
		}

		return result;
	}

	void replace_in_place(std::string& string, std::string_view const old_substr, std::string_view const new_substr) noexcept
	{
		std::size_t i = 0;
		while ((i = string.find(old_substr, i)) != std::string::npos)
		{
			string.replace(i, old_substr.size(), new_substr);
			i += new_substr.size();
		}
	}

	auto make_string_view(char const first[], char const last[]) noexcept -> std::string_view
	{
		debug_assert(last >= first);
		auto const size = last - first;
		return std::string_view(first, size);
	}

	auto begin_ptr(std::string_view sv) noexcept -> char const *
	{
		return sv.data();
	}

	auto end_ptr(std::string_view sv) noexcept -> char const *
	{
		return sv.data() + sv.size();
	}

	auto count_lines(std::string_view text) noexcept -> int
	{
		// Empty string has 0 lines.
		if (text.empty())
			return 0;
		else
			return static_cast<int>(1 + std::count(begin(text), end(text), '\n'));
	}

	auto split_first_line(std::string_view text) noexcept -> std::pair<std::string_view, std::string_view>
	{
		auto const newline = text.find('\n');

		if (newline == std::string_view::npos)
			return { text, std::string_view() };
		else
			return { text.substr(0, newline), text.substr(newline + 1) };
	}

	auto extract_first_line(std::string_view & text) noexcept -> std::string_view
	{
		auto const [line, rest] = split_first_line(text);
		text = rest;
		return line;
	}

	auto ignore_empty_lines(std::string_view text) noexcept -> std::string_view
	{
		while (!text.empty())
		{
			auto const [line, rest] = split_first_line(text);
			if (!std::all_of(begin(line), end(line), is_whitespace))
				return text;
			text = rest;
		}
		return text;
	}

	auto ignore_starting_whitespace(std::string_view text) noexcept -> std::string_view
	{
		size_t i = 0;
		while (i < text.size() && is_whitespace_or_newline(text[i]))
			++i;
		return text.substr(i);
	}

	auto ignore_trailing_whitespace(std::string_view text) noexcept -> std::string_view
	{
		size_t i = text.size();
		while (i > 0 && is_whitespace_or_newline(text[i - 1]))
			--i;
		return text.substr(0, i);
	}

	auto ignore_whitespace(std::string_view text) noexcept -> std::string_view
	{
		return ignore_starting_whitespace(ignore_trailing_whitespace(text));
	}

	auto starts_with(std::string_view string, std::string_view prefix) noexcept -> bool
	{
		return std::equal(begin(prefix), end(prefix), string.data());
	}

	auto is_contained_in(std::string_view string, std::string_view substr) noexcept -> bool
	{
		return substr.data() >= string.data() && end_ptr(substr) <= end_ptr(string);
	}

	auto indent(int indentation_level) noexcept -> std::string
	{
		return std::string(indentation_level, '\t');
	}

	std::string_view parent_path(std::string_view path) noexcept
	{
		return path.substr(0, path.find_last_of("/\\"));
	}

	std::string_view filename(std::string_view path) noexcept
	{
		size_t start = path.find_last_of("/\\");
		if (start == std::string_view::npos)
			start = 0;
		else
			start++;
		return path.substr(start);
	}

	std::string_view stem(std::string_view path) noexcept
	{
		size_t start = path.find_last_of("/\\");
		// If there are no file path separators, it's a file in the current directory, so take the string from the beginning.
		if (start == std::string_view::npos)
			start = 0;
		else
			start++;
		const size_t end = path.find_last_of('.');
		if (end > start)
			return path.substr(start, end - start);
		else
			return {};
	}

	std::string_view extension(std::string_view path) noexcept
	{
		size_t start = path.find_last_of('.');
		if (start == std::string_view::npos)
			return {};
		else
			return path.substr(start + 1);
	}

	void c_string_copy(std::span<char> dest, char const src[])
	{
		debug_assert(src != nullptr);

		const auto dest_size = dest.size();
		debug_assert(dest_size > 0);

		size_t i = 0;
		for (; i < dest_size - 1 && src[i] != 0; ++i)
			dest[i] = src[i];
		dest[i] = '\0';
	}

	void c_string_copy(std::span<char> dest, std::string_view src)
	{
		size_t const name_length_truncated = std::min(src.size(), dest.size() - 1);
		std::copy(src.begin(), src.begin() + name_length_truncated, dest.begin());
		dest[name_length_truncated] = '\0';
	}

	static auto allowed_file_types_to_library_format(std::span<AllowedFileType const> extensions) -> std::vector<std::string>
	{
		// Translate extension filters to the shitty format the library asks for.
		// The format is as follows:
		// - Every odd entry in the vector is the description of the file type
		//   e.g. Portable Document Format (PDF)
		// - Every even entry in the vector is an extension expressed with a wildcard,
		//   referring to the previous entry in the vector
		//   e.g. *.pdf
		if (extensions.empty())
		{
			using namespace std::string_literals;

			return { "All Files"s, "*"s };
		}

		std::vector<std::string> extension_filters;
		extension_filters.reserve(extensions.size() * 2);

		for (AllowedFileType const& extension_info : extensions)
		{
			using namespace std::string_view_literals;

			std::string extension_description;
			extension_description.append(reinterpret_cast<char const*>(extension_info.description.data()), extension_info.description.size());

			if (!extension_info.extensions.empty())
				extension_description += " (*"sv;

			std::string extension_wildcard;

			for (auto const extension : extension_info.extensions)
			{
				debug_assert(!extension.empty() && extension[0] == u8'.');

				extension_wildcard += '*';
				extension_wildcard.append(reinterpret_cast<char const*>(extension.data()), extension.size());
				extension_wildcard += ' ';

				extension_description.append(reinterpret_cast<char const*>(extension.data()), extension.size());
				extension_description += ";*"sv;
			}

			if (!extension_info.extensions.empty())
			{
				debug_assert(!extension_wildcard.empty() && extension_wildcard.back() == ' ');
				extension_wildcard.pop_back();

				debug_assert(extension_description.size() >= 2); // ;*
				extension_description.pop_back();
				extension_description.pop_back();
				extension_description += ')';
			}

			extension_filters.emplace_back(std::move(extension_description));
			extension_filters.emplace_back(std::move(extension_wildcard));
		}

		return extension_filters;
	}

	static auto open_files_impl(BrowseOpenMultipleFileOptions options,  bool single_file) -> std::vector<std::string>
	{
		return pfd::open_file(
			std::string(reinterpret_cast<char const *>(options.title.data()), options.title.size()),
			{}, // default filename
			allowed_file_types_to_library_format(options.allowed_file_types),
			((single_file) ? pfd::opt::none : pfd::opt::multiselect)).result();
	}

	auto browse_open_single_file(BrowseOpenSingleFileOptions options) -> std::optional<std::filesystem::path>
	{
		auto const files = open_files_impl(BrowseOpenMultipleFileOptions{ .title = options.title, .allowed_file_types = options.allowed_file_types }, true);

		if (files.empty())
			return std::nullopt;

		auto const & file = files.front();

		// The library gives paths in UTF-8, so we need
		// to tell std::filesystem::path they are indeed UTF-8
		// to convert to the correct native encoding (mostly on windows)
		return std::filesystem::path(std::u8string_view(
			reinterpret_cast<char8_t const *>(file.data()),
			file.size()
		));
	}

	auto browse_open_multiple_files(BrowseOpenMultipleFileOptions options) -> std::vector<std::filesystem::path>
	{
		auto const files = open_files_impl(options, false);

		if (files.empty())
			return {};

		std::vector<std::filesystem::path> paths;
		paths.reserve(files.size());

		// The library gives paths in UTF-8, so we need
		// to tell std::filesystem::path they are indeed UTF-8
		// to convert to the correct native encoding (mostly on windows)
		std::transform(files.cbegin(), files.cend(), std::back_inserter(paths), [](std::string const & file)
		{
			return std::filesystem::path(std::u8string_view(
				reinterpret_cast<char8_t const *>(file.data()), file.size()
			));
		});

		return paths;
	}

	auto browse_save_file(BrowseSaveFileOptions options) -> std::optional<std::filesystem::path>
	{
		auto const default_path_utf8 = options.default_path.u8string();

		auto const file = pfd::save_file(
			std::string(reinterpret_cast<char const *>(options.title.data()), options.title.size()),
			std::string(reinterpret_cast<char const *>(default_path_utf8.data()), default_path_utf8.size()),
			allowed_file_types_to_library_format(options.allowed_file_types),
			((options.file_overwrite_warning) ? pfd::opt::force_overwrite : pfd::opt::none)
		).result();

		if (file.empty())
			return std::nullopt;

		return std::filesystem::path(std::u8string_view(
			reinterpret_cast<char8_t const *>(file.data()),
			file.size()
		));
	}

	auto browse_directory(BrowseDirectoryOptions options) -> std::optional<std::filesystem::path>
	{
		debug_assert(options.default_path.empty() || std::filesystem::is_directory(options.default_path));

		auto const default_path_utf8 = options.default_path.u8string();

		auto const file = pfd::select_folder(
			std::string(reinterpret_cast<char const *>(options.title.data()), options.title.size()),
			std::string(reinterpret_cast<char const *>(default_path_utf8.data()), default_path_utf8.size()),
			pfd::opt::none
		).result();

		if (file.empty())
			return std::nullopt;

		return std::filesystem::path(std::u8string_view(
			reinterpret_cast<char8_t const *>(file.data()),
			file.size()
		));
	}
} // namespace aeh

namespace aeh
{
	namespace detail
	{
		auto view_as_utf8_advance(std::u8string_view text, size_t & index) noexcept -> char32_t
		{
			unsigned code_point_unsigned;
			int const chars_in_codepoint = ImTextCharFromUtf8(&code_point_unsigned, reinterpret_cast<char const *>(text.data()) + index, reinterpret_cast<char const *>(text.data()) + text.size());
			debug_assert_msg(chars_in_codepoint > 0, "String passed to render_text_justified is not well formed UTF-8.");

			const auto code_point = static_cast<char32_t>(code_point_unsigned);
			index += chars_in_codepoint;
			return code_point;
		}
	} // namespace detail

} // namespace aeh
