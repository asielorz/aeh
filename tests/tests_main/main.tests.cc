#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include "debug/assert.hh"

auto run_tests(int argc, char const * const argv[]) noexcept -> int
{
	// TODO: option for non-interactive
	int const result = Catch::Session().run(argc, argv);
	if (result != 0)
	{
		aeh::debug::message_box("A test failed", "One of the tests failed. Look at the console for more information.", aeh::debug::MBType::Ok | aeh::debug::MBIcon::Information);
		system_pause();
	}
	return result;
}

auto main(int argc, char const * const argv[]) -> int
{
	return run_tests(argc, argv);
}
