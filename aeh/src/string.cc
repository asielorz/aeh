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

#if AEH_WINDOWS
	bool browse(char buffer[], size_t size) noexcept
	{
		buffer[0] = '\0';

		//Open a file Dialog Box
		OPENFILENAME opf;
		opf.hwndOwner = 0;
		opf.lpstrFilter = NULL;
		opf.lpstrCustomFilter = 0;
		opf.nMaxCustFilter = 0L;
		opf.nFilterIndex = 1L;
		opf.lpstrFile = buffer;
		opf.lpstrFile[0] = '\0';
		opf.nMaxFile = static_cast<DWORD>(size - 2);
		opf.lpstrFileTitle = 0;
		opf.nMaxFileTitle = 50;
		opf.lpstrInitialDir = NULL;
		opf.lpstrTitle = NULL;
		opf.nFileOffset = 0;
		//opf.nFilemExtension    = 2;
		opf.lpstrDefExt = NULL;
		opf.lpfnHook = NULL;
		opf.lCustData = 0;
		opf.Flags = (DWORD)(OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_NOCHANGEDIR);
		opf.lStructSize = sizeof(OPENFILENAMEW);

		//Set the path of the file selected, if there's one
		return (GetOpenFileName(&opf) && buffer[0] != '\0');
	}
#endif

	std::vector<std::string> browse()
	{
#if !AEH_WINDOWS
		// TODO: Finish me
		return pfd::open_file("Open file selection", "my_replay.replay").result();
#else
		std::vector<std::string> selected_files;

		char buffer[1024];

		//Set the path of the file selected, if there's one
		if (browse(buffer, 1024))
		{
			// Replace all null characters for '?' in order to operate on them with string algorithms. Use '?' because it's a reserved character so we know a path will never contain it
			std::replace(std::begin(buffer), std::end(buffer) - 1, '\0', '?');
			const std::string data = { buffer, buffer + 1024 };

			// Check if there's a single path or more than one
			const size_t first_end = data.find('?');
			const size_t end = data.find("??");

			// If there's a single path, return it
			if (first_end == end)
			{
				buffer[end] = '\0';
				selected_files.push_back(buffer);
			}
			// For more than one path, windows gives the output in the format folder?path1?path2?...pathn??
			else
			{
				// Extract the folder in order to compose paths as folder + filename
				const std::string folder = std::string(data.substr(0, first_end)) + '\\';

				// Extract all filenames in a loop
				size_t next_end = first_end;
				while (next_end != end)
				{
					const size_t begin = next_end + 1;
					next_end = data.find('?', begin);
					const std::string filename = data.substr(begin, next_end - begin);
					selected_files.push_back(folder + filename);
				}
			}
		}

		return selected_files;
#endif
	}

	std::string browse_single_file()
	{
		return pfd::save_file("Replay export selection", "my_replay.replay", { "World of Traps replay files", "*.replay" }).result();
		//auto result = pfd::open_file("Replay export selection", "my_replay.replay"/*, { "All Files", "*" }*/).result();
		//if (result.size() > 0)
		//	return std::move(result[0]);

		//return {};

		//char buffer[1024];

		////Set the path of the file selected, if there's one
		//if (browse(buffer, 1024))
		//{
		//	// Check if there's a single path or more than one
		//	const std::string data = { buffer, buffer + 1024 };
		//	const size_t first_end = data.find('\0');
		//	const char * const double_null = "\0\0";
		//	const size_t end = data.find(std::string(double_null, double_null + 2));

		//	// If there's a single path, return it
		//	if (first_end == end)
		//	{
		//		buffer[end] = '\0';
		//		return buffer;
		//	}
		//}

		//return {};
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
