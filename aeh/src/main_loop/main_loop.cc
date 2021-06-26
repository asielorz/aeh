#ifdef AEH_WITH_SDL2

#include "main_loop.hh"

#include "gl/gl_core_4_5.hh"
#include "debug/assert.hh"
#include <SDL.h>
#include <thread>

namespace aeh::main_loop
{
	static bool sdl_initialized = false;

	bool initialize_sdl(uint32_t const extra_sdl_init_flags)
	{
		static_assert(std::is_same_v<std::remove_const_t<decltype(extra_sdl_init_flags)>, Uint32>, "Type mismatch between aeh and SDL");

		debug_assert_msg(!sdl_initialized, "Initializing SDL multiple times");

		// Setup SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | extra_sdl_init_flags) != 0)
		{
			debug::message_box("SDL_Init failed", ("SDL_Init failed. Error: " + std::string(SDL_GetError())).c_str(), debug::MBIcon::Error | debug::MBType::Ok);
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

		sdl_initialized = true;

		return true;
	}
} // namespace aeh::main_loop

namespace
{
	bool load_opengl_functions()
	{
		const gl::exts::LoadTest loaded = gl::sys::LoadFunctions();
		if (!loaded)
		{
			aeh::debug::message_box("OpenGL initialization failed", ("Failed to initialize OpenGL.\nNumber of functions that failed to load: " + std::to_string(loaded.GetNumMissing())).c_str(), aeh::debug::MBIcon::Error | aeh::debug::MBType::Ok);
			return false;
		}

		const int numFunctionsFailed = loaded.GetNumMissing();
		if (numFunctionsFailed > 0)
			std::fprintf(stderr, "Number of gl functions that failed to load: %d\n", numFunctionsFailed);

		return true;
	}

	void set_initial_opengl_configuration()
	{
		// Back face removal
		gl::Enable(gl::CULL_FACE);
		// Depth buffer
		gl::Disable(gl::DEPTH_TEST);
		// Alpha blending
		gl::Enable(gl::BLEND);
		gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);

		gl::ClearColor(0, 0, 0, 1);
	}

	aeh::main_loop::Window create_window(aeh::main_loop::NewWindowOptions const & options)
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

	bool update(SDL_Window * window, function_ref<void(SDL_Event const &)> demo_process_event)
	{
		bool done = false;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			demo_process_event(event);
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
			{
				done = true;
				SDL_PushEvent(&event);
				break;
			}
		}

		return done;
	}

	RenderInput pre_render(SDL_Window * window, Options const & options)
	{
		int window_width, window_height;
		SDL_GetWindowSize(window, &window_width, &window_height);
		auto const viewport = Viewport{0, 0, window_width, window_height};
		set_as_current_viewport(viewport);
		if (options.clear_every_frame)
			gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

		return RenderInput(window, viewport);
	}

	void post_render(SDL_Window * window)
	{
		SDL_GL_SwapWindow(window);
	}

	float cap_fps(std::chrono::steady_clock::time_point time_start) noexcept
	{
		constexpr auto minimum_frame_duration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(1.0 / 60.0));

		for (;;)
		{
			auto const frame_duration = std::chrono::steady_clock::now() - time_start;
			if (frame_duration < minimum_frame_duration)
				std::this_thread::yield();
			else
				return std::chrono::duration<float>(frame_duration).count();
		}
	}

} // namespace aeh::main_loop::detail

namespace aeh::main_loop
{
	Window Window::open_new(NewWindowOptions const & options)
	{
		debug_assert_msg(sdl_initialized, "Must call aeh::initialize_sdl() before calling aeh::main_loop::open_new()");
		if (!sdl_initialized)
			return {};

		Window new_window = create_window(options);

		if ((!new_window.window) || (!new_window.opengl_context))
			return {};

		static bool gl_init_ok = load_opengl_functions();
		if (!gl_init_ok)
			return {};

		set_initial_opengl_configuration();

		return new_window;
	}

	Window::~Window()
	{
		if (is_open())
		{
			SDL_GL_DeleteContext(opengl_context);
			SDL_DestroyWindow(window);
			if (previous_opengl_window && previous_opengl_context)
				SDL_GL_MakeCurrent(previous_opengl_window, previous_opengl_context);
		}
	}

	Window::Window(Window && other) noexcept
		: window(other.window)
		, opengl_context(other.opengl_context)
		, previous_opengl_window(other.previous_opengl_window)
		, previous_opengl_context(other.previous_opengl_context)
	{
		other.window = nullptr;
		other.opengl_context = nullptr;
		other.previous_opengl_window = nullptr;
		other.previous_opengl_context = nullptr;
	}

	Window & Window::operator = (Window && other) noexcept
	{
		window = other.window;
		opengl_context = other.opengl_context;
		previous_opengl_window = other.previous_opengl_window;
		previous_opengl_context = other.previous_opengl_context;
		other.window = nullptr;
		other.opengl_context = nullptr;
		other.previous_opengl_window = nullptr;
		other.previous_opengl_context = nullptr;

		return *this;
	}
}

#endif // AEH_WITH_SDL2
