#include "string.hh"
#include <catch2/catch.hpp>
#include <array>

TEST_CASE("replace with single chars")
{
	constexpr std::string_view source = "some test string";
	constexpr std::string_view old = "s";
	constexpr std::string_view new_ = "z";
	constexpr std::string_view expected = "zome tezt ztring";

	REQUIRE(aeh::replace(source, old, new_) == expected);

	std::string str(source);
	aeh::replace_in_place(str, old, new_);
	REQUIRE(str == expected);
}

TEST_CASE("replace with nothing")
{
	constexpr std::string_view source = "some test string";
	constexpr std::string_view old = "s";
	constexpr std::string_view new_ = "";
	constexpr std::string_view expected = "ome tet tring";

	REQUIRE(aeh::replace(source, old, new_) == expected);

	std::string str(source);
	aeh::replace_in_place(str, old, new_);
	REQUIRE(str == expected);
}

TEST_CASE("replace of different sizes")
{
	constexpr std::string_view source = "some test string";
	constexpr std::string_view old = "st";
	constexpr std::string_view new_ = "asdf";
	constexpr std::string_view expected = "some teasdf asdfring";

	REQUIRE(aeh::replace(source, old, new_) == expected);

	std::string str(source);
	aeh::replace_in_place(str, old, new_);
	REQUIRE(str == expected);
}

TEST_CASE("replace doesn't find anything")
{
	constexpr std::string_view source = "some test string";
	constexpr std::string_view old = "foo";
	constexpr std::string_view new_ = "bar";
	constexpr std::string_view expected = "some test string";

	REQUIRE(aeh::replace(source, old, new_) == expected);

	std::string str(source);
	aeh::replace_in_place(str, old, new_);
	REQUIRE(str == expected);
}

template <typename F>
static auto to_vector(aeh::generator<F> g) -> std::vector<typename aeh::generator<F>::value_type>
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
	std::vector<char32_t> const code_points = to_vector(aeh::view_as_utf8(text));

	REQUIRE(std::equal(text.begin(), text.end(), code_points.begin(), code_points.end()));
}

TEST_CASE("Non ascii chars take more than one byte")
{
	constexpr std::u8string_view text = u8"eñe";
	std::vector<char32_t> const code_points = to_vector(aeh::view_as_utf8(text));

	REQUIRE(code_points.size() < text.size());
	REQUIRE(code_points.size() == 3);
	REQUIRE(code_points[0] == U'e');
	REQUIRE(code_points[1] == U'ñ');
	REQUIRE(code_points[2] == U'e');
}

TEST_CASE("c_string_copy copies like strcpy but with a max buffer size")
{
	constexpr size_t source_size = 10;
	constexpr char source[] = "123456789\0This part shouldn't be accessible";

	constexpr char padding_char = char{ 1 };

	{
		char dest[source_size];
		std::fill(std::begin(dest), std::end(dest), padding_char);

		SECTION("Fits")
		{
			aeh::c_string_copy(dest, source);
			REQUIRE(std::string_view(std::data(dest), source_size) == std::string_view(std::data(source), source_size));
		}

		SECTION("Doesn't fit")
		{
			aeh::c_string_copy(std::span<char>(dest, std::size(dest) - 2), source);
			REQUIRE(std::string_view(std::data(dest), std::size(dest) - 2) == std::string(std::string_view(std::data(source), source_size - 3)) + '\0');
		}
	}

	// Fits with extra space: the extra space is not written to
	{
		char dest_big[source_size + 5];
		std::fill(std::begin(dest_big), std::end(dest_big), padding_char);

		aeh::c_string_copy(dest_big, source);
		REQUIRE(std::string_view(std::data(dest_big), source_size) == std::string_view(std::data(source), source_size));

		constexpr auto remaining_size = std::size(dest_big) - source_size;
		REQUIRE(std::string_view(std::data(dest_big) + source_size, remaining_size) == std::string(remaining_size, padding_char));
	}
}
