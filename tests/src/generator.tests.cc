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
