#pragma once

#ifdef AEH_WITH_SDL2

#include "optional_customization_points.hh"
#include "function_ref.hh"
#include <type_traits> // std::is_same_v
#include <tuple>
#include <optional>

struct SDL_Window;
union SDL_Event;

namespace aeh::main_loop
{
	struct Options
	{
		bool clear_every_frame = true;
	};

	struct NewWindowOptions : Options
	{
		char const * window_name = "New window";
		int window_width = 1280;
		int window_height = 720;
		bool can_resize_window = true;
	};
	
	struct UpdateInput
	{
		UpdateInput(float dt_, SDL_Window * window_, bool & done_, int & exit_code_) noexcept 
			: dt(dt_)
			, window(window_)
			, done(done_)
			, exit_code(exit_code_)
		{}

		float const dt;
		SDL_Window * const window;

		void quit(int exit_code_ = 0) noexcept { done = true; exit_code = exit_code_; }

	protected:
		bool & done;
		int & exit_code;
	};

	template <typename Demo>
	int run(Demo & demo, SDL_Window * window, Options const & options = Options());

	template <typename Demo>
	int run(Demo & demo, NewWindowOptions const & options = NewWindowOptions());

} // namespace demo_main_loop

#include "main_loop.inl"

#endif // AEH_WITH_SDL2
