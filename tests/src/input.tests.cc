#include "input/input.hh"
#include <catch2/catch.hpp>

static auto make_control_state_naive(bool currently_down, bool previously_down) noexcept -> aeh::in::ControlState
{
	if (currently_down)
	{
		if (previously_down)
			return aeh::in::ControlState::down;
		else
			return aeh::in::ControlState::pressed;
	}
	else
	{
		if (previously_down)
			return aeh::in::ControlState::released;
		else
			return aeh::in::ControlState::up;
	}
}

TEST_CASE("Make control state")
{
	REQUIRE(aeh::in::make_control_state(false, false) == make_control_state_naive(false, false));
	REQUIRE(aeh::in::make_control_state(false, true) == make_control_state_naive(false, true));
	REQUIRE(aeh::in::make_control_state(true, false) == make_control_state_naive(true, false));
	REQUIRE(aeh::in::make_control_state(true, true) == make_control_state_naive(true, true));
}
