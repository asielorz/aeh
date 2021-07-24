#include "msp/minimalistic_shared_ptr.hh"
#include "catch2/catch.hpp"

TEST_CASE("Default constructed shared_ptr is null")
{
	aeh::msp::shared_ptr<int> p;
	REQUIRE(p == nullptr);
}

TEST_CASE("shared_ptr can be constructed from nullptr and result is null")
{
	aeh::msp::shared_ptr<int> p = nullptr;
	REQUIRE(p == nullptr);
}

TEST_CASE("make_new makes a new heap allocated object and returns a shared_ptr that manages it")
{
	aeh::msp::shared_ptr<int> p = aeh::msp::shared_ptr<int>::make_new(5);
	REQUIRE(p != nullptr);
	REQUIRE(*p == 5);
	REQUIRE(p.is_unique());
	REQUIRE(p.use_count() == 1);
}

TEST_CASE("shared_ptr can be copied")
{
	aeh::msp::shared_ptr<int> a = aeh::msp::shared_ptr<int>::make_new(5);
	auto b = a;

	REQUIRE(a == b);
	REQUIRE(*a == 5);
	REQUIRE(*b == 5);
	REQUIRE(!a.is_unique());
	REQUIRE(!b.is_unique());
	REQUIRE(a.use_count() == 2);
	REQUIRE(b.use_count() == 2);

	// a and b share the same actual memory
	*a = 6;
	REQUIRE(*b == 6);
}

TEST_CASE("shared_ptr can be copy assigned")
{
	aeh::msp::shared_ptr<int> a = aeh::msp::shared_ptr<int>::make_new(5);
	aeh::msp::shared_ptr<int> b;
	b = a;

	REQUIRE(a == b);
	REQUIRE(*a == 5);
	REQUIRE(*b == 5);
	REQUIRE(!a.is_unique());
	REQUIRE(!b.is_unique());
	REQUIRE(a.use_count() == 2);
	REQUIRE(b.use_count() == 2);

	// a and b share the same actual memory
	*a = 6;
	REQUIRE(*b == 6);
}

TEST_CASE("shared_ptr can be moved")
{
	aeh::msp::shared_ptr<int> a = aeh::msp::shared_ptr<int>::make_new(5);

	REQUIRE(a != nullptr);
	REQUIRE(*a == 5);

	auto b = std::move(a);

	REQUIRE(a != b);
	REQUIRE(a == nullptr);
	REQUIRE(b != nullptr);

	REQUIRE(*b == 5);
	REQUIRE(b.is_unique());
	REQUIRE(b.use_count() == 1);
}

TEST_CASE("shared_ptr can be move assigned")
{
	aeh::msp::shared_ptr<int> a = aeh::msp::shared_ptr<int>::make_new(5);
	aeh::msp::shared_ptr<int> b;

	REQUIRE(a != nullptr);
	REQUIRE(*a == 5);
	REQUIRE(b == nullptr);

	b = std::move(a);

	REQUIRE(a != b);
	REQUIRE(a == nullptr);
	REQUIRE(b != nullptr);

	REQUIRE(*b == 5);
	REQUIRE(b.is_unique());
	REQUIRE(b.use_count() == 1);
}

TEST_CASE("shared_ptr behaves correctly when assigning pointers with same value")
{
	aeh::msp::shared_ptr<int> a = aeh::msp::shared_ptr<int>::make_new(5);
	auto b = a;
	b = a;
	b = a;
	a = b;
	b = a;
	a = b;
	a = b;
	a = b;

	REQUIRE(b != nullptr);
	REQUIRE(*a == 5);
	REQUIRE(*b == 5);
	REQUIRE(a.use_count() == 2);
	REQUIRE(b.use_count() == 2);
}

TEST_CASE("shared_ptr can be self assigned")
{
	aeh::msp::shared_ptr<int> a = aeh::msp::shared_ptr<int>::make_new(5);
	a = a;

	REQUIRE(*a == 5);
	REQUIRE(a.is_unique());
	REQUIRE(a.use_count() == 1);
}

TEST_CASE("shared_ptr can be self move assigned")
{
	aeh::msp::shared_ptr<int> a = aeh::msp::shared_ptr<int>::make_new(5);
	a = std::move(a);

	REQUIRE(*a == 5);
	REQUIRE(a.is_unique());
	REQUIRE(a.use_count() == 1);
}

TEST_CASE("A shared_ptr can point to a const object")
{
	aeh::msp::shared_ptr<int const> a = aeh::msp::shared_ptr<int const>::make_new(5);

	REQUIRE(*a == 5);
	REQUIRE(a.is_unique());
	REQUIRE(a.use_count() == 1);

	static_assert(std::is_const_v<std::remove_reference_t<decltype(*a)>>);
}

TEST_CASE("A shared_ptr to const can be constructed from a shared_ptr to mutable")
{
	aeh::msp::shared_ptr<int> a = aeh::msp::shared_ptr<int>::make_new(5);
	aeh::msp::shared_ptr<int const> b = a;

	REQUIRE(*b == 5);
	*a = 10;
	REQUIRE(*b == 10);
}

TEST_CASE("shared_ptr to const can be self assigned")
{
	auto a = aeh::msp::shared_ptr<int const>::make_new(5);
	a = a;

	REQUIRE(*a == 5);
	REQUIRE(a.is_unique());
	REQUIRE(a.use_count() == 1);
}

TEST_CASE("shared_ptr to const can be self move assigned")
{
	auto a = aeh::msp::shared_ptr<int const>::make_new(5);
	a = std::move(a);

	REQUIRE(*a == 5);
	REQUIRE(a.is_unique());
	REQUIRE(a.use_count() == 1);
}

TEST_CASE("Move assignment from mutable to const shared_ptr")
{
	aeh::msp::shared_ptr<int> a = aeh::msp::shared_ptr<int>::make_new(5);
	aeh::msp::shared_ptr<int const> b;

	b = std::move(a);

	REQUIRE(a == nullptr);
	REQUIRE(b != nullptr);
	REQUIRE(*b == 5);
	REQUIRE(b.is_unique());
}

TEST_CASE("nullptr assignment")
{
	aeh::msp::shared_ptr<int> a = aeh::msp::shared_ptr<int>::make_new(5);
	a = nullptr;
	REQUIRE(a == nullptr);

	aeh::msp::shared_ptr<int const> b = aeh::msp::shared_ptr<int const>::make_new(5);
	b = nullptr;
	REQUIRE(b == nullptr);
}

TEST_CASE("A shared_ptr with an empty deleter has the same size as a pointer")
{
	static_assert(sizeof(aeh::msp::shared_ptr<int>) == sizeof(void *));
}
