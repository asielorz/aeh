#include <catch2/catch_session.hpp>

#include "debug/assert.hh"
#include "tests_main.hh"

namespace aeh
{
	auto run_tests(int argc, char const * const argv[]) noexcept -> int
	{
		// TODO: option for non-interactive
		int const result = Catch::Session().run(argc, argv);
		if (result != 0)
		{
			aeh::debug::message_box("A test failed", "One of the tests failed. Look at the console for more information.", aeh::debug::MBType::Ok | aeh::debug::MBIcon::Information);
			aeh::debug::system_pause();
		}
		return result;
	}
} // namespace aeh

auto main(int argc, char const * const argv[]) -> int
{
	return aeh::run_tests(argc, argv);
}
