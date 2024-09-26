#include "algorithm.hh"
#include "fixed_capacity_vector.hh"
#include <catch2/catch_test_macros.hpp>

using test_vector = aeh::fixed_capacity_vector<int, 8>;

TEST_CASE("aeh::erase removes all elements with the given value from a container that supports remove + erase idiom")
{
	test_vector v = {1, 2, 3, 4, 1, 1, 3, 4};
	size_t const erased_count = aeh::erase(v, 1);
	REQUIRE(v == test_vector{2, 3, 4, 3, 4});
	REQUIRE(erased_count == 3);
}

TEST_CASE("aeh::erase_if removes all elements that satisfy a predicate from a container that supports remove + erase idiom")
{
	test_vector v = {1, 2, 3, 4, 1, 1, 3, 4};
	size_t const erased_count = aeh::erase_if(v, [](int i) { return i % 2 == 0; });
	REQUIRE(v == test_vector{1, 3, 1, 1, 3});
	REQUIRE(erased_count == 3);
}

TEST_CASE("aeh::update_then_erase_if calls the given potentially mutating function on all elements, then removes those for which the function returns true")
{
	test_vector v = {1, 2, 3, 4, 1, 1, 3, 4};
	size_t const erased_count = aeh::update_then_erase_if(v, [](int & i) { i--; return i == 0; });
	REQUIRE(v == test_vector{1, 2, 3, 2, 3});
	REQUIRE(erased_count == 3);
}
