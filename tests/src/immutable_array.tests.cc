#include "msp/immutable_array.hh"
#include "catch2/catch.hpp"

TEST_CASE("Default constructed immutable_array is empty")
{
	aeh::msp::immutable_array<int> a;
	REQUIRE(a.empty());
	REQUIRE(a.size() == 0);
}

TEST_CASE("immutable_array can be constructed from initializer list")
{
	aeh::msp::immutable_array<int> a = aeh::msp::immutable_array<int>::from_ilist({1, 2, 3, 4});
	REQUIRE(!a.empty());
	REQUIRE(a.size() == 4);
	REQUIRE(a[0] == 1);
	REQUIRE(a[1] == 2);
	REQUIRE(a[2] == 3);
	REQUIRE(a[3] == 4);
}

TEST_CASE("immutable_array can be constructed as a copy of a range")
{
	int some_array[] = {-5, 0, 7, -3, 4};
	aeh::msp::immutable_array<int> a = aeh::msp::immutable_array<int>::copy_of_range(std::begin(some_array), std::end(some_array));
	REQUIRE(!a.empty());
	REQUIRE(a.size() == 5);
	REQUIRE(a[0] == -5);
	REQUIRE(a[1] ==  0);
	REQUIRE(a[2] ==  7);
	REQUIRE(a[3] == -3);
	REQUIRE(a[4] ==  4);
}

TEST_CASE("immutable_array can be constructed by moving a range")
{
	std::unique_ptr<int> some_ptrs[] = {
		std::make_unique<int>(1),
		std::make_unique<int>(0),
		std::make_unique<int>(-1),
		std::make_unique<int>(7)
	};
	aeh::msp::immutable_array<std::unique_ptr<int>> a = aeh::msp::immutable_array<std::unique_ptr<int>>::move_from_range(std::begin(some_ptrs), std::end(some_ptrs));

	REQUIRE(!a.empty());
	REQUIRE(a.size() == 4);
	REQUIRE(*a[0] ==  1);
	REQUIRE(*a[1] ==  0);
	REQUIRE(*a[2] == -1);
	REQUIRE(*a[3] ==  7);

	REQUIRE(some_ptrs[0] == nullptr);
	REQUIRE(some_ptrs[1] == nullptr);
	REQUIRE(some_ptrs[2] == nullptr);
	REQUIRE(some_ptrs[3] == nullptr);
}

TEST_CASE("front and back access the first and last elements")
{
	auto a = aeh::msp::immutable_array<int>::from_ilist({1, 2, 3, 4});

	REQUIRE(a.front() == 1);
	REQUIRE(a.back() == 4);
}

TEST_CASE("immutable_array is convertible to const span")
{
	auto a = aeh::msp::immutable_array<int>::from_ilist({1, 2, 3, 4});
	std::span<int const> s = a;

	REQUIRE(s.size() == 4);
	REQUIRE(s[0] == 1);
	REQUIRE(s[1] == 2);
	REQUIRE(s[2] == 3);
	REQUIRE(s[3] == 4);
}

TEST_CASE("immutable_array can be copied")
{
	auto a = aeh::msp::immutable_array<int>::from_ilist({1, 2, 3, 4});
	auto b = a;

	REQUIRE(a == b);

	REQUIRE(a.size() == 4);
	REQUIRE(a[0] == 1);
	REQUIRE(a[1] == 2);
	REQUIRE(a[2] == 3);
	REQUIRE(a[3] == 4);

	REQUIRE(b.size() == 4);
	REQUIRE(b[0] == 1);
	REQUIRE(b[1] == 2);
	REQUIRE(b[2] == 3);
	REQUIRE(b[3] == 4);
}

TEST_CASE("immutable_array can be moved")
{
	auto a = aeh::msp::immutable_array<int>::from_ilist({1, 2, 3, 4});
	auto b = std::move(a);

	REQUIRE(a != b);

	REQUIRE(a.empty());
	REQUIRE(a.size() == 0);

	REQUIRE(b.size() == 4);
	REQUIRE(b[0] == 1);
	REQUIRE(b[1] == 2);
	REQUIRE(b[2] == 3);
	REQUIRE(b[3] == 4);
}

TEST_CASE("immutable_array can be copy assigned")
{
	auto a = aeh::msp::immutable_array<int>::from_ilist({1, 2, 3, 4});
	aeh::msp::immutable_array<int> b;
	b = a;

	REQUIRE(a == b);

	REQUIRE(a.size() == 4);
	REQUIRE(a[0] == 1);
	REQUIRE(a[1] == 2);
	REQUIRE(a[2] == 3);
	REQUIRE(a[3] == 4);

	REQUIRE(b.size() == 4);
	REQUIRE(b[0] == 1);
	REQUIRE(b[1] == 2);
	REQUIRE(b[2] == 3);
	REQUIRE(b[3] == 4);
}

TEST_CASE("immutable_array can be move assigned")
{
	auto a = aeh::msp::immutable_array<int>::from_ilist({1, 2, 3, 4});
	aeh::msp::immutable_array<int> b;
	b = std::move(a);

	REQUIRE(a != b);

	REQUIRE(a.empty());
	REQUIRE(a.size() == 0);

	REQUIRE(b.size() == 4);
	REQUIRE(b[0] == 1);
	REQUIRE(b[1] == 2);
	REQUIRE(b[2] == 3);
	REQUIRE(b[3] == 4);
}

TEST_CASE("immutable_array is a random access contiguous range")
{
	auto a = aeh::msp::immutable_array<int>::from_ilist({0, 1, 2, 3, 4, 5, 6, 7, 8});

	auto it = std::lower_bound(a.begin(), a.end(), 5);
	REQUIRE(it == a.nth(5));

	static_assert(std::is_same_v <
		std::iterator_traits<aeh::msp::immutable_array<int>::iterator>::iterator_category,
		std::random_access_iterator_tag
	>);

	static_assert(std::is_same_v <
		std::iterator_traits<aeh::msp::immutable_array<int>::iterator>::iterator_concept,
		std::contiguous_iterator_tag
	>);
}

TEST_CASE("transform takes a function vector<T> -> vector<T> and returns a new immutable_array with the same value as the vector returned by transform")
{
	auto a = aeh::msp::immutable_array<int>::from_ilist({1, 2, 3, 4});
	auto b = a.transform([](std::vector<int> v)
	{
		v.push_back(5);
		v.push_back(6);
		v.push_back(7);
		v.push_back(8);
		return v;
	});

	REQUIRE(b.size() == 8);
	REQUIRE(b[0] == 1);
	REQUIRE(b[1] == 2);
	REQUIRE(b[2] == 3);
	REQUIRE(b[3] == 4);
	REQUIRE(b[4] == 5);
	REQUIRE(b[5] == 6);
	REQUIRE(b[6] == 7);
	REQUIRE(b[7] == 8);
}

TEST_CASE("permute takes a function that takes a span<T> and may mutate it, then returns a new immutable_array with the same value as the span at the end of the function")
{
	auto a = aeh::msp::immutable_array<int>::from_ilist({6, 1, 7, 2, 5, 3, 0, 4, 8});
	auto b = a.permute([](std::span<int> s) { std::sort(s.begin(), s.end()); });

	REQUIRE(b.size() == 9);
	REQUIRE(b[0] == 0);
	REQUIRE(b[1] == 1);
	REQUIRE(b[2] == 2);
	REQUIRE(b[3] == 3);
	REQUIRE(b[4] == 4);
	REQUIRE(b[5] == 5);
	REQUIRE(b[6] == 6);
	REQUIRE(b[7] == 7);
	REQUIRE(b[8] == 8);
}

//**********************************************************************************************************************

TEST_CASE("immutable_array_builder can be constructed with a given size")
{
	auto builder = aeh::msp::immutable_array_builder<int>::with_size(16);

	REQUIRE(builder.size() == 16);
}

TEST_CASE("immutable_array_builder can be constructed from an initializer list")
{
	auto builder = aeh::msp::immutable_array_builder<int>::from_ilist({0, 1, 2, 3});

	REQUIRE(builder.size() == 4);
	REQUIRE(builder[0] == 0);
	REQUIRE(builder[1] == 1);
	REQUIRE(builder[2] == 2);
	REQUIRE(builder[3] == 3);
}

TEST_CASE("Contents of the builder are mutable")
{
	auto builder = aeh::msp::immutable_array_builder<int>::from_ilist({ 3, 1, 4, 2, 0 });

	std::sort(builder.begin(), builder.end());

	REQUIRE(builder.size() == 5);
	REQUIRE(builder[0] == 0);
	REQUIRE(builder[1] == 1);
	REQUIRE(builder[2] == 2);
	REQUIRE(builder[3] == 3);
	REQUIRE(builder[4] == 4);
}

TEST_CASE("Builder can be built from a range")
{
	int some_array[] = {-5, 0, 7, -3, 4};
	auto a = aeh::msp::immutable_array_builder<int>::copy_of_range(std::begin(some_array), std::end(some_array));
	REQUIRE(a.size() == 5);
	REQUIRE(a[0] == -5);
	REQUIRE(a[1] ==  0);
	REQUIRE(a[2] ==  7);
	REQUIRE(a[3] == -3);
	REQUIRE(a[4] ==  4);
}

TEST_CASE("Builder can be built from moving a range")
{
	std::unique_ptr<int> some_ptrs[] = {
		std::make_unique<int>(1),
		std::make_unique<int>(0),
		std::make_unique<int>(-1),
		std::make_unique<int>(7)
	};
	auto a = aeh::msp::immutable_array_builder<std::unique_ptr<int>>::move_from_range(std::begin(some_ptrs), std::end(some_ptrs));

	REQUIRE(a.size() == 4);
	REQUIRE(*a[0] ==  1);
	REQUIRE(*a[1] ==  0);
	REQUIRE(*a[2] == -1);
	REQUIRE(*a[3] ==  7);

	REQUIRE(some_ptrs[0] == nullptr);
	REQUIRE(some_ptrs[1] == nullptr);
	REQUIRE(some_ptrs[2] == nullptr);
	REQUIRE(some_ptrs[3] == nullptr);
}

TEST_CASE("finish returns an immutable_array and empties the builder")
{
	auto builder = aeh::msp::immutable_array_builder<int>::from_ilist({3, 1, 4, 2, 0});

	std::sort(builder.begin(), builder.end());

	REQUIRE(builder.size() == 5);
	REQUIRE(builder[0] == 0);
	REQUIRE(builder[1] == 1);
	REQUIRE(builder[2] == 2);
	REQUIRE(builder[3] == 3);
	REQUIRE(builder[4] == 4);

	REQUIRE(!builder.has_finished());
	aeh::msp::immutable_array<int> array = builder.finish();
	REQUIRE(builder.has_finished());

	REQUIRE(array.size() == 5);
	REQUIRE(array[0] == 0);
	REQUIRE(array[1] == 1);
	REQUIRE(array[2] == 2);
	REQUIRE(array[3] == 3);
	REQUIRE(array[4] == 4);
}

TEST_CASE("default constructed builder is in finished state")
{
	aeh::msp::immutable_array_builder<int> builder;
	REQUIRE(builder.has_finished());
}

TEST_CASE("immutable_array_builder cannot be copied")
{
	static_assert(!std::is_copy_constructible_v<aeh::msp::immutable_array_builder<int>>);
	static_assert(!std::is_copy_assignable_v<aeh::msp::immutable_array_builder<int>>);
}

TEST_CASE("immutable_array_builder can be move constructed")
{
	auto builder = aeh::msp::immutable_array_builder<int>::from_ilist({ 3, 1, 4, 2, 0 });

	std::sort(builder.begin(), builder.end());

	REQUIRE(builder.size() == 5);
	REQUIRE(builder[0] == 0);
	REQUIRE(builder[1] == 1);
	REQUIRE(builder[2] == 2);
	REQUIRE(builder[3] == 3);
	REQUIRE(builder[4] == 4);

	auto second = std::move(builder);

	REQUIRE(second.size() == 5);
	REQUIRE(second[0] == 0);
	REQUIRE(second[1] == 1);
	REQUIRE(second[2] == 2);
	REQUIRE(second[3] == 3);
	REQUIRE(second[4] == 4);

	// A builder that has been moved from is left in the finished state.
	REQUIRE(builder.has_finished());
}

TEST_CASE("immutable_array_builder can be move assigned")
{
	auto builder = aeh::msp::immutable_array_builder<int>::from_ilist({ 3, 1, 4, 2, 0 });

	std::sort(builder.begin(), builder.end());

	REQUIRE(builder.size() == 5);
	REQUIRE(builder[0] == 0);
	REQUIRE(builder[1] == 1);
	REQUIRE(builder[2] == 2);
	REQUIRE(builder[3] == 3);
	REQUIRE(builder[4] == 4);

	aeh::msp::immutable_array_builder<int> second;
	second = std::move(builder);

	REQUIRE(second.size() == 5);
	REQUIRE(second[0] == 0);
	REQUIRE(second[1] == 1);
	REQUIRE(second[2] == 2);
	REQUIRE(second[3] == 3);
	REQUIRE(second[4] == 4);

	// A builder that has been moved from is left in the finished state.
	REQUIRE(builder.has_finished());
}
