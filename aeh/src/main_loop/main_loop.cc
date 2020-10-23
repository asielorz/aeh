#ifdef AEH_HAS_SDL

#include "main_loop.hh"

#include "gl/gl_core_4_5.hh"
#include <SDL.h>

namespace
{
	bool initialize_SDL()
	{
		// Setup SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
		{
			printf("Error: %s\n", SDL_GetError());
			return false;
		}

		// Setup window
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);
		SDL_GL_SetSwapInterval(1); // Enable vsync
		SDL_GameControllerEventState(SDL_ENABLE); // Enable controllers sending events.

		return true;
	}

	bool load_opengl_functions()
	{
		const gl::exts::LoadTest loaded = gl::sys::LoadFunctions();
		if (!loaded)
			return false;

		const int numFunctionsFailed = loaded.GetNumMissing();
		if (numFunctionsFailed > 0)
			printf("Number of gl functions that failed to load: %d\n", numFunctionsFailed);

		// Back face removal
		gl::Enable(gl::CULL_FACE);
		// Depth buffer
		gl::Disable(gl::DEPTH_TEST);
		// Alpha blending
		gl::Enable(gl::BLEND);
		gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);

		gl::ClearColor(0, 0, 0, 1);

		return true;
	}

	aeh::main_loop::detail::LoopVars create_window(aeh::main_loop::Options const & options)
	{
		SDL_Window * const prev_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext const prev_context = SDL_GL_GetCurrentContext();
		unsigned window_flags = SDL_WINDOW_OPENGL;
		if (options.can_resize_window)
			window_flags |= SDL_WINDOW_RESIZABLE;
		SDL_Window * const window = SDL_CreateWindow(options.window_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, options.window_width, options.window_height, window_flags);
		SDL_GLContext const gl_context = SDL_GL_CreateContext(window);
		return {window, gl_context, prev_window, prev_context};
	}
} // namespace

namespace aeh::main_loop::detail
{

	LoopVars initialize(Options const & options)
	{
		static bool sdl_init_ok = initialize_SDL();
		if (!sdl_init_ok)
			return {};

		LoopVars const vars = create_window(options);

		if ((!vars.window) || (!vars.opengl_context))
			return {};

		static bool gl_init_ok = load_opengl_functions();
		if (!gl_init_ok)
			return {};

		return vars;
	}

	std::tuple<bool, float> update(SDL_Window * window, std::function<void(SDL_Event const &)> demo_process_event)
	{
		bool done = false;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			demo_process_event(event);
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}

		return {done, 0.01666f};
	}

	void pre_render(SDL_Window * window)
	{
		int window_width, window_height;
		SDL_GetWindowSize(window, &window_width, &window_height);
		gl::Viewport(0, 0, window_width, window_height);
		gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
	}

	void post_render(SDL_Window * window)
	{
		SDL_GL_SwapWindow(window);
	}

	void shutdown(LoopVars vars)
	{
		SDL_GL_DeleteContext(vars.opengl_context);
		SDL_DestroyWindow(vars.window);
		if (vars.previous_opengl_window && vars.previous_opengl_context)
			SDL_GL_MakeCurrent(vars.previous_opengl_window, vars.previous_opengl_context);
	}

} // namespace demo_main_loop

#endif // AEH_HAS_SDL
