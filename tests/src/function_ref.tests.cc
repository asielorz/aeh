#include "function_ref.hh"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Default constructed function_ref is null")
{
	aeh::function_ref<int(int)> const f;
	REQUIRE(!f.has_value());
}

TEST_CASE("function_ref can be constructed from a callable")
{
	auto l = [](int i) { return i; };
	aeh::function_ref<int(int)> const f = l;
	REQUIRE(f.has_value());
}

TEST_CASE("The callable can be invoked through the function_ref")
{
	auto l = [i = 0]() mutable { return i++; };
	aeh::function_ref<int()> const f = l;
	REQUIRE(f() == 0);
	REQUIRE(l() == 1);
	REQUIRE(f() == 2);
	REQUIRE(l() == 3);
	REQUIRE(f() == 4);
	REQUIRE(l() == 5);
}

TEST_CASE("Construction and comparison with nullptr")
{
	aeh::function_ref<int(int)> const f_default;
	aeh::function_ref<int(int)> const f_null = nullptr;

	REQUIRE(f_default == nullptr);
	REQUIRE(!(f_default != nullptr));
	REQUIRE(f_null == nullptr);
	REQUIRE(!(f_null != nullptr));

	REQUIRE(nullptr == f_default);
	REQUIRE(!(nullptr != f_default));
	REQUIRE(nullptr == f_null);
	REQUIRE(!(nullptr != f_null));

	auto l = [i = 0]() mutable { return i++; };
	aeh::function_ref<int()> const f_value = l;

	REQUIRE(!(f_value == nullptr));
	REQUIRE(f_value != nullptr);
	REQUIRE(!(f_value == nullptr));
	REQUIRE(f_value != nullptr);
}

TEST_CASE("Two function refs compare equal if they point at the same object or if both are null")
{
	aeh::function_ref<int()> const f_default;
	aeh::function_ref<int()> const f_null = nullptr;

	REQUIRE(f_default == f_null);

	auto l1 = [i = 0]() mutable { return i++; };
	aeh::function_ref<int()> const f_value1 = l1;
	aeh::function_ref<int()> const f_value2 = l1;
	aeh::function_ref<int()> const f_value_copy = f_value1;

	REQUIRE(f_value1 == f_value2);
	REQUIRE(f_value1 == f_value1);
	REQUIRE(f_value1 == f_value_copy);
	REQUIRE(f_value2 == f_value_copy);
	REQUIRE(f_value1 != f_default);

	auto l2 = [i = 0]() mutable { return i++; };
	aeh::function_ref<int()> const f_different_value = l2;

	REQUIRE(f_value1 != f_different_value);
	REQUIRE(f_different_value != f_default);
}

TEST_CASE("Reference to a constant callable")
{
	auto const l = [](int i) { return i; };
	aeh::function_ref<int(int)> const f = l;
}

namespace tests
{
	constexpr int add(int a, int b) noexcept
	{
		return a + b;
	}
}
/*
TEST_CASE("to_constant allows creating a function_ref to a function_ptr without needing the function_ptr object")
{
	auto f = aeh::function_ref<int(int, int)>::to_constant<tests::add>;

	REQUIRE(f(3, 4) == 7);
}

TEST_CASE("function_ref objects created with to_constant can be called at compile time")
{
	constexpr auto f = aeh::function_ref<int(int, int)>::to_constant<tests::add>;

	STATIC_REQUIRE(f(3, 4) == 7);
}

TEST_CASE("function_ref to member function")
{
	struct TestStruct
	{
		int i;

		constexpr int get_i() const noexcept { return i; }
	};

	constexpr auto f = aeh::function_ref<int(TestStruct)>::to_constant<&TestStruct::get_i>;

	constexpr auto s = TestStruct{5};

	STATIC_REQUIRE(f(s) == 5);
}

TEST_CASE("function_ref to member pointer")
{
	struct TestStruct
	{
		int i;
	};

	constexpr auto f = aeh::function_ref<int(TestStruct)>::to_constant<&TestStruct::i>;

	constexpr auto s = TestStruct{ 5 };

	STATIC_REQUIRE(f(s) == 5);
}

TEST_CASE("function_ref is trivial")
{
	STATIC_REQUIRE(std::is_trivially_destructible_v<aeh::function_ref<int()>>);
	STATIC_REQUIRE(std::is_trivially_copyable_v<aeh::function_ref<int()>>);
}

TEST_CASE("noexcept function_ref")
{
	constexpr auto f = aeh::function_ref<int(int, int) noexcept>::to_constant<tests::add>;
	STATIC_REQUIRE(noexcept(f(3, 4)));
}
*/
