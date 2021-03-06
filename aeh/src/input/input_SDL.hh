#pragma once

#ifdef AEH_WITH_SDL2

union SDL_Event;

namespace aeh::in
{

	struct Frame;
	struct PreviousFrame;

	auto prepare_frame_to_process_SDL_events(PreviousFrame const & prev) noexcept -> Frame;
	auto process_SDL_event(Frame & frame, SDL_Event const & event) noexcept -> void;

} // namespace aeh::in

#endif // AEH_WITH_SDL2
