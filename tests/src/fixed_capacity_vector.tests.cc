#include "fixed_capacity_vector.hh"
#include <catch2/catch.hpp>

using test_vector = aeh::fixed_capacity_vector<int, 8>;

TEST_CASE("Default constructed vector is empty")
{
	test_vector v;
	REQUIRE(v.size() == 0);
	REQUIRE(v.empty());
}

TEST_CASE("Constructor from initializer list")
{
	test_vector v = {1, 2, 3, 4};
	REQUIRE(v.size() == 4);
	REQUIRE(v[0] == 1);
	REQUIRE(v[1] == 2);
	REQUIRE(v[2] == 3);
	REQUIRE(v[3] == 4);
}

TEST_CASE("Conversion to span")
{
	test_vector v = {1, 2, 3, 4};
	aeh::span<int> s = v;

	REQUIRE(s.size() == 4);
	REQUIRE(s[0] == 1);
	REQUIRE(s[1] == 2);
	REQUIRE(s[2] == 3);
	REQUIRE(s[3] == 4);
}

TEST_CASE("erase single element")
{
	test_vector v = {1, 2, 3, 4};

	{
		auto const it = v.erase(v.begin());
		REQUIRE(v == test_vector{2, 3, 4});
		REQUIRE(*it == 2);
	}

	{
		auto const it = v.erase(v.nth(1));
		REQUIRE(v == test_vector{2, 4});
		REQUIRE(*it == 4);
	}
}

TEST_CASE("Erasing last element returns end")
{
	test_vector v = {1, 2, 3, 4};
	auto const it = v.erase(v.nth(3));
	REQUIRE(v == test_vector{1, 2, 3});
	REQUIRE(it == v.end());
}

TEST_CASE("erasing a range")
{
	test_vector v = {1, 2, 3, 4, 5, 6, 7, 8};
	auto const it = v.erase(v.begin(), v.nth(3));
	REQUIRE(v == test_vector{4, 5, 6, 7, 8});
	REQUIRE(*it == 4);

	v.erase(v.begin(), v.end());
	REQUIRE(v.empty());
}
