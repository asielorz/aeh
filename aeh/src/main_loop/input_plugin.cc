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

	auto InputPlugin::start_frame() const -> in::Frame
	{
		return prepare_frame_to_process_SDL_events(input.previous);
	}

	auto InputPlugin::update(UpdateInput, in::Frame const & current_frame) -> InputExtension
	{
		input = in::update(input, current_frame);
		return InputExtension{input};
	}
	
	auto InputPlugin::process_event(SDL_Event const & event, in::Frame & current_frame) -> void
	{
		process_SDL_event(current_frame, event);
	}

} // namespace aeh::main_loop
