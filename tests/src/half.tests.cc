#include "half.hh"
#include "randomly_seeded.hh"
#include <catch2/catch.hpp>

TEST_CASE("A half can be converted to float and back without changing")
{
	using aeh::half;

	auto rng = aeh::randomly_seeded<std::mt19937>();
	auto random_state = std::uniform_int_distribution<uint16_t>();

	for (int iterations = 0; iterations < 10000; ++iterations)
	{
		half const h1 = half::with_state(random_state(rng));
		half const h2 = half::from_float(h1.as_float());
		REQUIRE(h1 == h2);
	}
}
