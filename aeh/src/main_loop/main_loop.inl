namespace aeh::main_loop
{

	namespace detail
	{
		struct LoopVars
		{
			LoopVars() noexcept = default;
			LoopVars(
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

			SDL_Window * window = nullptr;
			void * opengl_context = nullptr;
			SDL_Window * previous_opengl_window = nullptr;
			void * previous_opengl_context = nullptr;
		};

		LoopVars initialize(NewWindowOptions const & options);
		bool update(SDL_Window * window, function_ref<void(SDL_Event const &)> demo_process_event);
		void pre_render(SDL_Window * window, Options const & options);
		void post_render(SDL_Window * window);
		float cap_fps(std::chrono::steady_clock::time_point time_start) noexcept;
		void shutdown(LoopVars vars);
	} // namespace detail

	template <typename Demo>
	int run(Demo & demo, SDL_Window * window, Options const & options)
	{
		main_loop::detail::call_initialize(demo, window);

		// Loop
		bool done = false;
		int exit_code = 0;
		float dt = 1.0f / 60.0f;
		while (!done)
		{
			auto const time_start = std::chrono::steady_clock::now();

			auto locals = main_loop::detail::call_start_frame(demo);

			// Update
			done = main_loop::detail::update(window,
				[&demo, &locals](SDL_Event const & ev) { main_loop::detail::call_process_event(demo, ev, locals); });

			auto controller = main_loop::UpdateInput(dt, window, done, exit_code);
			main_loop::detail::call_update(demo, controller, locals);

			// Rendering
			if (!done)
			{
				main_loop::detail::pre_render(window, options);
				main_loop::detail::render_demo(demo, locals);
				main_loop::detail::post_render(window);

				dt = main_loop::detail::cap_fps(time_start);
			}
		}

		main_loop::detail::call_shutdown(demo);

		return exit_code;
	}

	template <typename Demo>
	int run(Demo & demo, NewWindowOptions const & options)
	{
		// Initialize
		main_loop::detail::LoopVars loop_vars = main_loop::detail::initialize(options);
		if (!loop_vars.window)
			return -1;

		int const result = run(demo, loop_vars.window, options);
		main_loop::detail::shutdown(loop_vars);
		return result;
	}

} // namespace aeh::main_loop
