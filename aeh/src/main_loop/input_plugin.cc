#ifdef AEH_WITH_SDL2

#include "input_plugin.hh"
#include "input/input_SDL.hh"
#include "main_loop.hh"
#include <SDL2/SDL.h>

namespace aeh::main_loop
{

	auto InputPlugin::initialize(SDL_Window * window) -> void
	{
		int window_width, window_height;
		SDL_GetWindowSize(window, &window_width, &window_height);
		input.current.window_width = static_cast<uint16_t>(window_width);
		input.current.window_height = static_cast<uint16_t>(window_height);
	}

	auto InputPlugin::start_frame() const noexcept -> in::Frame
	{
		return prepare_frame_to_process_SDL_events(input.current);
	}

	auto InputPlugin::update(UpdateInput loop, in::Frame const & current_frame) noexcept -> InputExtension
	{
		// If a frame takes too long (for an arbitrary definition of too long), reset the input to compensate for possible missed input events.
		constexpr float frame_duration = 1.0f / 60.0f;
		if (loop.dt > 10 * frame_duration)
		{
			input = {};
			controller_repeater = {};
		}
		else
		{
			input = in::update(input, current_frame);
			in::update(controller_repeater, loop.dt, input);
		}
		return InputExtension{input, controller_repeater};
	}
	
	auto InputPlugin::process_event(SDL_Event const & event, in::Frame & current_frame) noexcept -> void
	{
		process_SDL_event(current_frame, event);
	}

} // namespace aeh::main_loop

#endif // AEH_WITH_SDL2
