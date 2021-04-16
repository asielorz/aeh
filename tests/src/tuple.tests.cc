#include "tuple.hh"
#include "string.hh"
#include <catch2/catch.hpp>

TEST_CASE("tuple_at calls a function at the ith element")
{
	auto const tup = std::tuple(1u, -42, false);
	auto const f = [](auto x) { return aeh::to_string(x); };

	REQUIRE(aeh::tuple_at(tup, 0, f) == "1");
	REQUIRE(aeh::tuple_at(tup, 1, f) == "-42");
	REQUIRE(aeh::tuple_at(tup, 2, f) == "false");
}
