namespace aeh::main_loop
{

	namespace detail
	{
		bool update(SDL_Window * window, function_ref<void(SDL_Event const &)> demo_process_event);
		RenderInput pre_render(SDL_Window * window, Options const & options);
		void post_render(SDL_Window * window);
		float cap_fps(std::chrono::steady_clock::time_point time_start) noexcept;
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
				RenderInput const render_input = main_loop::detail::pre_render(window, options);
				main_loop::detail::render_demo(demo, render_input, locals);
				main_loop::detail::post_render(window);

				dt = main_loop::detail::cap_fps(time_start);
			}
		}

		main_loop::detail::call_shutdown(demo);

		return exit_code;
	}

	template <typename Demo, typename = std::enable_if_t<std::is_rvalue_reference_v<Demo &&>>>
	int run(Demo && demo, SDL_Window * window, Options const & options)
	{
		Demo & demo_as_non_rvalue_ref = demo;
		return run(demo_as_non_rvalue_ref, window, options);
	}

	template <typename Demo>
	int run(Demo & demo, NewWindowOptions const & options)
	{
		// Initialize
		main_loop::Window new_window = main_loop::Window::open_new(options);
		if (!new_window.is_open())
			return -1;

		int const result = run(demo, new_window.window, options);
		return result;
	}

	template <typename Demo, typename = std::enable_if_t<std::is_rvalue_reference_v<Demo &&>>>
	int run(Demo && demo, NewWindowOptions const & options)
	{
		Demo & demo_as_non_rvalue_ref = demo;
		return run(demo_as_non_rvalue_ref, options);
	}

} // namespace aeh::main_loop
