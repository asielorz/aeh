#ifdef AEH_WITH_SDL2

#include "input_plugin.hh"
#include "input/input_SDL.hh"
#include "main_loop.hh"
#include <SDL2/SDL.h>

namespace aeh::main_loop
{
	namespace
	{
		thread_local InputPlugin * current = nullptr;
	}

	auto InputPlugin::initialize(SDL_Window * window) -> void
	{
		if (current)
		{
			input = current->input;
		}
		else
		{
			int window_width, window_height;
			SDL_GetWindowSize(window, &window_width, &window_height);
			input.current.window_width = static_cast<uint16_t>(window_width);
			input.current.window_height = static_cast<uint16_t>(window_height);
		}
		previous = current;
		current = this;
	}

	auto InputPlugin::shutdown() -> void
	{
		if (previous)
			previous->input = input;
		current = previous;
	}

	auto InputPlugin::start_frame() const noexcept -> in::Frame
	{
		return prepare_frame_to_process_SDL_events(input.current);
	}

	static void reset_after_inactivity(in::Input & input, in::Frame const & current_frame)
	{
		// Do not reset fields that do not make sense to reset, like mpuse position and window size. It is unlikely that a freeze
		// caused mouse position or window size to become (0, 0). Instead, update them regularly to account for changes to these
		// during the freeze and have the newest possible values.
		int16_t const mouse_pos_x = input.current.mouse_pos_x;
		int16_t const mouse_pos_y = input.current.mouse_pos_y;
		int16_t const window_width = input.current.window_width;
		int16_t const window_height = input.current.window_height;
		input = {};
		input.current.mouse_pos_x = current_frame.mouse_pos_x;
		input.current.mouse_pos_y = current_frame.mouse_pos_y;
		input.current.window_width = current_frame.window_width;
		input.current.window_height = current_frame.window_height;
		input.previous.mouse_pos_x = mouse_pos_x;
		input.previous.mouse_pos_y = mouse_pos_y;
		input.previous.window_width = window_width;
		input.previous.window_height = window_height;
	}

	auto InputPlugin::update(UpdateInput loop, in::Frame const & current_frame) noexcept -> InputExtension
	{
		// If a frame takes too long (for an arbitrary definition of too long), reset the input to compensate for possible missed input events.
		constexpr float frame_duration = 1.0f / 60.0f;
		if (loop.dt > 10 * frame_duration)
		{
			reset_after_inactivity(input, current_frame);
			controller_repeater = {};
		}
		else
		{
			input = in::update(input, current_frame);
			controller_repeater = in::update(controller_repeater, loop.dt, input);
		}
		return InputExtension{input, controller_repeater};
	}
	
	auto InputPlugin::process_event(SDL_Event const & event, in::Frame & current_frame) noexcept -> void
	{
		process_SDL_event(current_frame, event);
	}

} // namespace aeh::main_loop

#endif // AEH_WITH_SDL2
