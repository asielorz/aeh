#include "polymorphic_generator.hh"
#include "string.hh"
#include <catch2/catch.hpp>

TEST_CASE("Polymorphic generator is a type erased version of the generator")
{
	aeh::polymorphic_generator<std::string_view> split_generator = aeh::split("En un lugar de la Mancha", ' ');

	std::vector<std::string_view> v;
	for (std::string_view part : split_generator)
		v.push_back(part);

	REQUIRE(v == std::vector<std::string_view>{"En", "un", "lugar", "de", "la", "Mancha"});
}

TEST_CASE("A generator may generate references")
{
	std::array<int, 4> array = {1, 2, 3, 4};

	auto g = aeh::generator([&array, i = size_t(0)]() mutable -> int *
	{
		if (i >= std::size(array))
			return nullptr;

		return &array[i++];
	});

	for (int & i : g)
		i += 1;

	REQUIRE(array == std::array<int, 4>{2, 3, 4, 5});
}
