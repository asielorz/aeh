#include "string.hh"
#include <catch2/catch.hpp>
#include <array>

TEST_CASE("replace with single chars")
{
	std::string s = aeh::replace("some test string", "s", "z");
	REQUIRE(s == "zome tezt ztring");
}

TEST_CASE("replace with nothing")
{
	std::string s = aeh::replace("some test string", "s", "");
	REQUIRE(s == "ome tet tring");
}

TEST_CASE("replace of different sizes")
{
	std::string s = aeh::replace("some test string", "st", "asdf");
	REQUIRE(s == "some teasdf asdfring");
}

TEST_CASE("replace doesn't find anything")
{
	std::string s = aeh::replace("some test string", "foo", "bar");
	REQUIRE(s == "some test string");
}

template <typename StringView, typename Delimiter, std::output_iterator<StringView> Output>
void split_to(Output output, StringView text, Delimiter delimiter)
{
	for (auto part : aeh::split(text, delimiter))
		*output++ = part;
}

template <typename StringView, typename Delimiter>
std::vector<StringView> split_to_vector(StringView text, Delimiter delimiter, std::size_t reserve = 0)
{
	std::vector<StringView> result;
	result.reserve(reserve);

	split_to(std::back_inserter(result), text, delimiter);

	return result;
}

TEST_CASE("split single char")
{
	using namespace std::string_view_literals;

	auto const expected = std::vector<std::string_view>{"128"sv, "0"sv, "0"sv, "1"sv};

	constexpr std::string_view text = "128.0.0.1"sv;

	REQUIRE(split_to_vector(text, '.') == expected);
	REQUIRE(split_to_vector(text, "."sv) == expected);

	{
		std::vector<std::string_view> vec;
		vec.reserve(4);
		split_to(std::back_inserter(vec), text, '.');
		REQUIRE(vec == expected);
	}

	{
		std::array<std::string_view, 4> array;
		REQUIRE(expected.size() == 4);

		split_to(array.begin(), text, '.');
		REQUIRE(array == (std::array{expected[0], expected[1], expected[2], expected[3]}));
	}

	{
		std::list<std::string_view> list;
		split_to(std::back_inserter(list), text, "."sv);
		REQUIRE(list == std::list(expected.cbegin(), expected.cend()));
	}
}

TEST_CASE("Split that ends with a delimiter")
{
	using namespace std::string_view_literals;

	auto const expected = std::vector<std::string_view>{"std", "chrono", "literals"sv, ""sv};
	constexpr std::string_view text = "std::chrono::literals::";

	auto const splitted = split_to_vector(text, "::"sv);

	REQUIRE(splitted == expected);
}
