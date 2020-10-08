#include <catch2/catch.hpp>
#include "span.hh"

using aeh::span;

TEST_CASE("Can construct span from pointer and size")
{
	int some_array[] = { 1, 2, 3 };
	auto const s = span<int>(some_array, 3);
	REQUIRE(s.size() == 3);
	REQUIRE(s[0] == 1);
	REQUIRE(s[1] == 2);
	REQUIRE(s[2] == 3);
}

TEST_CASE("Can construct span from array")
{
	int some_array[] = { 1, 2, 3 };
	auto const s = span<int>(some_array);
	REQUIRE(s.size() == 3);
	REQUIRE(s[0] == 1);
	REQUIRE(s[1] == 2);
	REQUIRE(s[2] == 3);
}

TEST_CASE("const span")
{
	int const some_array[] = { 1, 2, 3 };
	auto const s = span<int const>(some_array);
	REQUIRE(s.size() == 3);
	REQUIRE(s[0] == 1);
	REQUIRE(s[1] == 2);
	REQUIRE(s[2] == 3);
}

// For whatever reason, if the constexpr array
// that is used to construct span is a local
// variable, clang and gcc say the operation is
// not constexpr. Same happens with std::span:
// https://gcc.godbolt.org/z/jnY3b3
constexpr int some_constexpr_array[] = { 1, 2, 3 };
TEST_CASE("constexpr span")
{
	constexpr auto s = span<int const>(some_constexpr_array);
	static_assert(s.size() == 3);
	static_assert(s[0] == 1);
	static_assert(s[1] == 2);
	static_assert(s[2] == 3);
}

TEST_CASE("Range for")
{
	int const some_array[] = { 1, 2, 3 };
	auto const s = span<int const>(some_array);
	int sum = 0;
	for (int i : s)
		sum += i;
	REQUIRE(sum == 6);
}

TEST_CASE("Reverse iterators")
{
	int const some_array[] = { 1, 2, 3 };
	auto const s = span<int const>(some_array);

	int const reversed_array[] = { 3, 2, 1 };

	REQUIRE(std::equal(s.rbegin(), s.rend(), std::begin(reversed_array), std::end(reversed_array)));
}
