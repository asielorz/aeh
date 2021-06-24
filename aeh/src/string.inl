#include "string.hh"
namespace aeh
{

	template <typename T>
	auto to_string(T const & t) noexcept -> decltype(std::to_string(t))
	{
		return std::to_string(t);
	}

	constexpr auto to_string(std::string const & s) noexcept -> std::string const &
	{
		return s;
	}

	inline auto to_string(char const * s) noexcept -> std::string
	{
		return s;
	}

	inline auto to_string(std::string_view s) noexcept -> std::string
	{
		return std::string(s);
	}

	inline auto to_string(bool b) noexcept -> std::string
	{
		return b ? "true" : "false";
	}

	inline auto to_string(char c) noexcept -> std::string
	{
		return std::string(1, c);
	}

	template <typename ... Ts>
	auto join(Ts const & ... ts) noexcept -> std::string
	{
		return (to_string(ts) + ...);
	}

	namespace detail
	{
		template <typename T>
		concept has_size = requires(T const t) { {t.size()} -> std::convertible_to<size_t>; };

		template <typename StringView, typename Delimiter>
		constexpr auto split_impl(StringView text, Delimiter delimiter)
		{
			return generator([text, delimiter, index = size_t(0)]() mutable noexcept -> std::optional<StringView>
			{
				if (index > text.size())
					return std::nullopt;

				size_t next = text.find(delimiter, index);
				if (next == std::string_view::npos)
					next = text.size();
				StringView const substring = text.substr(index, next - index);

				if constexpr (detail::has_size<Delimiter>)
					index = next + delimiter.size();
				else
					index = next + 1;

				return substring;
			});
		}
	} // namespace detail

	constexpr auto split(std::string_view text, char delimiter) noexcept
	{
		return detail::split_impl(text, delimiter);
	}

	constexpr auto split(std::string_view text, std::string_view delimiter) noexcept
	{
		return detail::split_impl(text, delimiter);
	}

	constexpr auto split(std::u8string_view text, char8_t delimiter) noexcept
	{
		return detail::split_impl(text, delimiter);
	}

	constexpr auto split(std::u8string_view text, std::u8string_view delimiter) noexcept
	{
		return detail::split_impl(text, delimiter);
	}

	namespace detail
	{
		auto view_as_utf8_advance(std::u8string_view text, size_t & index) noexcept -> char32_t;
	}

	inline auto view_as_utf8(std::u8string_view text) noexcept
	{
		return generator([text, index = size_t(0)]() mutable noexcept -> std::optional<char32_t>
		{
			if (index >= text.size())
				return std::nullopt;

			return detail::view_as_utf8_advance(text, index);
		});
	}

} // namespace aeh