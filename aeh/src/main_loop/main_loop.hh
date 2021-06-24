#pragma once

#ifdef AEH_WITH_SDL2

#include "optional_customization_points.hh"
#include "function_ref.hh"
#include "viewport.hh"
#include <type_traits> // std::is_same_v
#include <tuple>
#include <optional>
#include <chrono>

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

	struct Window
	{
		Window() noexcept = default;
		Window(
			SDL_Window * window_,
			void * opengl_context_,
			SDL_Window * previous_opengl_window_,
			void * previous_opengl_context_
		) noexcept
			: window(window_)
			, opengl_context(opengl_context_)
			, previous_opengl_window(previous_opengl_window_)
			, previous_opengl_context(previous_opengl_context_)
		{}

		static Window open_new(NewWindowOptions const &);
		~Window();

		Window(Window const &) noexcept = delete;
		Window & operator = (Window const &) noexcept = delete;

		Window(Window && other) noexcept;
		Window & operator = (Window && other) noexcept;

		bool is_open() const noexcept { return window != nullptr; }
		bool has_previous_window() const noexcept { return previous_opengl_window != nullptr; }

		SDL_Window * window = nullptr;
		void * opengl_context = nullptr;
		SDL_Window * previous_opengl_window = nullptr;
		void * previous_opengl_context = nullptr;
	};
	
	struct UpdateInput
	{
		explicit UpdateInput(float dt_, SDL_Window * window_, bool & done_, int & exit_code_) noexcept 
			: dt(dt_)
			, window(window_)
			, done(done_)
			, exit_code(exit_code_)
		{}

		float const dt;
		SDL_Window * const window;

		void quit(int exit_code_ = 0) const noexcept { done = true; exit_code = exit_code_; }

	protected:
		bool & done;
		int & exit_code;
	};

	struct RenderInput
	{
		explicit RenderInput(SDL_Window * const window_, Viewport const viewport_) noexcept
			: window(window_)
			, viewport(viewport_)
		{}

		SDL_Window * const window;
		Viewport const viewport;
	};

	template <typename Demo>
	int run(Demo & demo, SDL_Window * window, Options const & options = Options());

	template <typename Demo> requires std::is_rvalue_reference_v<Demo&&>
	int run(Demo && demo, SDL_Window * window, Options const & options = Options());

	template <typename Demo>
	int run(Demo & demo, NewWindowOptions const & options = NewWindowOptions());

	template <typename Demo> requires std::is_rvalue_reference_v<Demo&&>
	int run(Demo && demo, NewWindowOptions const & options = NewWindowOptions());

} // namespace demo_main_loop

#include "main_loop.inl"

#endif // AEH_WITH_SDL2
