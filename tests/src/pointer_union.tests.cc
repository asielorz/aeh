#include "pointer_union.hh"
#include <catch.hpp>

using pointer_t = aeh::PointerUnion<int, uint16_t>;

TEST_CASE("Initializing from null")
{
	pointer_t const p = nullptr;
	CHECK(!p);
	CHECK(p.pointer() == nullptr);
	CHECK(p.extra_data() == 0);
}

TEST_CASE("Initializing from pointer")
{
	int i = 5;
	pointer_t const p = pointer_t(&i);
	CHECK(static_cast<bool>(p));
	CHECK(p.pointer() == &i);
	CHECK(p.extra_data() == 0);
}

TEST_CASE("Initializing from extra value")
{
	pointer_t const p = pointer_t(uint16_t(45));
	CHECK(!p);
	CHECK(p.pointer() == nullptr);
	CHECK(p.extra_data() == 45);
}

TEST_CASE("Initializing from pointer and extra value")
{
	int i = 5;
	pointer_t const p = pointer_t(&i, uint16_t(45));
	CHECK(static_cast<bool>(p));
	CHECK(p.pointer() == &i);
	CHECK(p.extra_data() == 45);
}
