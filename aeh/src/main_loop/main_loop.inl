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

		LoopVars initialize(Options const & options);
		std::tuple<bool, float> update(SDL_Window * window, function_ref<void(SDL_Event const &)> demo_process_event);
		void pre_render(SDL_Window * window);
		void post_render(SDL_Window * window);
		void shutdown(LoopVars vars);
	} // namespace detail

	template <typename Demo>
	int run(Demo & demo, SDL_Window * window)
	{
		main_loop::detail::call_initialize(demo, window);

		// Loop
		bool done = false;
		int exit_code = 0;
		while (!done)
		{
			auto locals = main_loop::detail::call_start_frame(demo);

			// Update
			auto const [done_, dt] = main_loop::detail::update(window,
				[&demo, &locals](SDL_Event const & ev) { main_loop::detail::call_process_event(demo, ev, locals); });

			done = done_;
			auto controller = main_loop::UpdateInput(dt, window, done, exit_code);
			main_loop::detail::call_update(demo, controller, locals);

			// Rendering
			main_loop::detail::pre_render(window);
			main_loop::detail::render_demo(demo, locals);
			main_loop::detail::post_render(window);
		}

		main_loop::detail::call_shutdown(demo);

		return exit_code;
	}

	template <typename Demo>
	int run(Demo & demo, Options const & options)
	{
		// Initialize
		main_loop::detail::LoopVars loop_vars = main_loop::detail::initialize(options);
		if (!loop_vars.window)
			return -1;

		int const result = run(demo, loop_vars.window);
		main_loop::detail::shutdown(loop_vars);
		return result;
	}

} // namespace aeh::main_loop
