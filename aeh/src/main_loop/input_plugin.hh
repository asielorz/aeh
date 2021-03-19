#pragma once

#ifdef AEH_WITH_SDL2

#include "input/input.hh"
#include "input/repeat_controller.hh"

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
			in::ControllerRepeater const & controller_repeater;
		};

		InputPlugin() noexcept = default;

		auto initialize(SDL_Window * window) -> void;
		auto shutdown() -> void;

		auto start_frame() const noexcept -> in::Frame;
		auto update(UpdateInput i, in::Frame const & current_frame) noexcept -> InputExtension;
		auto process_event(SDL_Event const & event, in::Frame & current_frame) noexcept -> void;

	private:
		in::Input input;
		in::ControllerRepeater controller_repeater;
		InputPlugin * previous = nullptr;
	};

} // namespace aeh::main_loop

#endif // AEH_WITH_SDL2
