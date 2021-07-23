#include "repeat_controller.hh"
#include "debug/assert.hh"

namespace aeh::in
{

	void reset(ControllerRepeater::Entry & entry)
	{
		entry.value = 0;
		entry.repeating = false;
	}

	void loop(ControllerRepeater::Entry & entry)
	{
		entry.value = 0;
		entry.repeating = true;
	}

	auto is_ready(ControllerRepeater::Entry entry, float period, float first_wait) -> bool
	{
		if (entry.repeating)
			return entry.value >= period;
		else
			return entry.value >= first_wait;
	}

	auto update(ControllerRepeater const & previous, float dt, Input const & input) noexcept -> ControllerRepeater
	{
		ControllerRepeater repeater = previous;

		for (int controller_index = 0; controller_index < 4; ++controller_index)
		{
			Controller const controller = controller_enum_from_index(controller_index);

			auto & buttons = repeater.controllers[controller_index].buttons;
			auto & joysticks_positive = repeater.controllers[controller_index].joysticks_positive;
			auto & joysticks_negative = repeater.controllers[controller_index].joysticks_negative;
			auto & triggers = repeater.controllers[controller_index].triggers;

			for (int i = 0; i < controller_button_count; ++i)
			{
				auto const bound_button = controller | static_cast<ControllerButton>(i | binary_literal(1000'0000'0000'0000));

				if (is_ready(buttons[i], repeater.period, repeater.first_wait))
					loop(buttons[i]);

				if (is_down(input, bound_button))
					buttons[i].value += dt;
				else
					reset(buttons[i]);
			}

			for (int i = 0; i < 4; ++i)
			{
				auto const bound_axis = controller | static_cast<ControllerAxis>(i | binary_literal(1001'0000'0000'0000));

				if (is_ready(joysticks_positive[i], repeater.period, repeater.first_wait))
					loop(joysticks_positive[i]);
				if (is_ready(joysticks_negative[i], repeater.period, repeater.first_wait))
					loop(joysticks_negative[i]);

				if (is_down(input, bound_axis, repeater.threshold))
					joysticks_positive[i].value += dt;
				else
					reset(joysticks_positive[i]);

				if (is_down(input, bound_axis, -repeater.threshold))
					joysticks_negative[i].value += dt;
				else
					reset(joysticks_negative[i]);
			}

			for (int i = 0; i < 2; ++i)
			{
				auto const bound_axis = controller | static_cast<ControllerAxis>((i + 4) | binary_literal(1001'0000'0000'0000));

				if (is_ready(triggers[i], repeater.period, repeater.first_wait))
					loop(triggers[i]);

				if (is_down(input, bound_axis, repeater.threshold))
					triggers[i].value += dt;
				else
					reset(triggers[i]);
			}
		}
		
		return repeater;
	}

	auto is_pressed_or_repeated(Input const & input, BoundControllerButton button, ControllerRepeater const & repeater) noexcept -> bool
	{
		auto const controller_index = index_of(extract_controller(button));
		auto const button_index = index_of(extract_button(button));

		return 
			is_pressed(input, button) ||
			is_ready(repeater.controllers[controller_index].buttons[button_index], repeater.period, repeater.first_wait);
	}

	auto is_pressed_or_repeated_positive(Input const & input, BoundControllerAxis bound_axis, ControllerRepeater const & repeater) noexcept -> bool
	{
		ControllerAxis const axis = extract_axis(bound_axis);

		if (axis >= ControllerAxis::left_trigger)
			return
				is_pressed(input, bound_axis, repeater.threshold) ||
				is_ready(repeater.controllers[index_of(extract_controller(bound_axis))].triggers[index_of(axis) - 4], repeater.period, repeater.first_wait);
		else
			return
				is_pressed(input, bound_axis, repeater.threshold) ||
				is_ready(repeater.controllers[index_of(extract_controller(bound_axis))].joysticks_positive[index_of(axis)], repeater.period, repeater.first_wait);
	}

	auto is_pressed_or_repeated_negative(Input const & input, BoundControllerAxis bound_axis, ControllerRepeater const & repeater) noexcept -> bool
	{
		ControllerAxis const axis = extract_axis(bound_axis);
		debug_assert(axis < ControllerAxis::left_trigger);

		return
			is_pressed(input, bound_axis, -repeater.threshold) ||
			is_ready(repeater.controllers[index_of(extract_controller(bound_axis))].joysticks_negative[index_of(axis)], repeater.period, repeater.first_wait);
	}

} // namespace aeh::in
