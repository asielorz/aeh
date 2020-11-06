#pragma once

#ifdef AEH_WITH_SDL2

#include "input/input.hh"

struct SDL_Window;
union SDL_Event;

namespace aeh::main_loop
{
	struct UpdateInput;

	struct InputPlugin
	{
		struct InputExtension
		{
			in::Input const & input;
		};

		auto initialize(SDL_Window * window) -> void;
		auto start_frame() const noexcept -> in::Frame;
		auto update(UpdateInput i, in::Frame const & current_frame) noexcept -> InputExtension;
		auto process_event(SDL_Event const & event, in::Frame & current_frame) noexcept -> void;

	private:
		in::Input input;
	};

} // namespace aeh::main_loop

#endif // AEH_WITH_SDL2
