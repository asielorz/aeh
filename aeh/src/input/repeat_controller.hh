#pragma once

#include "input.hh"

namespace aeh::in
{

	struct ControllerRepeater
	{
		constexpr ControllerRepeater() noexcept = default;
		ControllerRepeater(float period_, float first_wait_, float threshold_) noexcept 
			: period(period_)
			, first_wait(first_wait_)
			, threshold(threshold_) 
		{}

		struct Entry
		{
			float value = 0.0f;
			bool repeating = false;
		};

		struct
		{
			Entry buttons[controller_button_count] = {0};
			Entry joysticks_positive[4] = {0};
			Entry joysticks_negative[4] = {0};
			Entry triggers[2] = {0};
		} controllers[4];

		float period = 0.1f;     //! Time between repeat triggers, except for the first one.
		float first_wait = 0.4f; //! Tile until it first repeat triggers once the button is down.
		float threshold = 0.5f;
	};

	void update(ControllerRepeater & repeater, float dt, Input const & input) noexcept;
	[[nodiscard]] auto is_pressed_or_repeated(Input const & input, BoundControllerButton button, ControllerRepeater const & repeater) noexcept -> bool;
	[[nodiscard]] auto is_pressed_or_repeated_positive(Input const & input, BoundControllerAxis button, ControllerRepeater const & repeater) noexcept -> bool;
	[[nodiscard]] auto is_pressed_or_repeated_negative(Input const & input, BoundControllerAxis button, ControllerRepeater const & repeater) noexcept -> bool;

} // namespace aeh::in
