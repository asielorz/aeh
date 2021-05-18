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

template <typename F>
auto to_vector(aeh::generator<F> g) -> std::vector<typename aeh::generator<F>::value_type>
{
	std::vector<typename aeh::generator<F>::value_type> result;
	result.reserve(16);

	for (auto && part : g)
		result.push_back(part);

	return result;
}

TEST_CASE("Split single char")
{
	using namespace std::string_view_literals;

	auto const expected = std::vector<std::string_view>{"128"sv, "0"sv, "0"sv, "1"sv};

	constexpr std::string_view text = "128.0.0.1"sv;

	REQUIRE(to_vector(aeh::split(text, '.')) == expected);
	REQUIRE(to_vector(aeh::split(text, "."sv)) == expected);
}

TEST_CASE("Split that ends with a delimiter")
{
	using namespace std::string_view_literals;

	auto const expected = std::vector<std::string_view>{"std", "chrono", "literals", ""};
	constexpr std::string_view text = "std::chrono::literals::";

	auto const splitted = to_vector(aeh::split(text, "::"sv));

	REQUIRE(splitted == expected);
}

TEST_CASE("Split that begins with a delimiter")
{
	using namespace std::string_view_literals;

	auto const expected = std::vector<std::string_view>{"", "std", "chrono", "literals"};
	constexpr std::string_view text = "::std::chrono::literals";

	auto const splitted = to_vector(aeh::split(text, "::"sv));

	REQUIRE(splitted == expected);
}

TEST_CASE("Split with multiple consecutive delimiters")
{
	auto const expected = std::vector<std::string_view>{ "", "", "", "2", "", "3", "0", "", "" };
	constexpr std::string_view text = "...2..3.0..";

	auto const splitted = to_vector(aeh::split(text, '.'));

	REQUIRE(splitted == expected);
}

constexpr auto compile_time_split(std::string_view str, char delim) noexcept -> std::array<std::string_view, 4>
{
	auto split_generator = aeh::split(str, delim);
	std::array<std::string_view, 4> result;
	result[0] = *split_generator();
	result[1] = *split_generator();
	result[2] = *split_generator();
	result[3] = *split_generator();
	return result;
}

TEST_CASE("Splitting at compile time")
{
	constexpr std::string_view text = "128.0.0.1";
	constexpr std::array<std::string_view, 4> splitted = compile_time_split(text, '.');
	STATIC_REQUIRE(splitted[0] == "128");
	STATIC_REQUIRE(splitted[1] == "0");
	STATIC_REQUIRE(splitted[2] == "0");
	STATIC_REQUIRE(splitted[3] == "1");
}

TEST_CASE("Ascii text is utf8")
{
	constexpr std::u8string_view text = u8"A good ol' ascii string!";
	std::vector<unsigned> const code_points = to_vector(aeh::view_as_utf8(text));

	REQUIRE(std::equal(text.begin(), text.end(), code_points.begin(), code_points.end()));
}

TEST_CASE("Non ascii chars take more than one byte")
{
	constexpr std::u8string_view text = u8"eñe";
	std::vector<unsigned> const code_points = to_vector(aeh::view_as_utf8(text));

	REQUIRE(code_points.size() < text.size());
	REQUIRE(code_points.size() == 3);
	REQUIRE(code_points[0] == u'e');
	REQUIRE(code_points[1] == u'ñ');
	REQUIRE(code_points[2] == u'e');
}
