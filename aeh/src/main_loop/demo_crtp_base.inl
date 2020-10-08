namespace aeh::main_loop
{
	namespace detail
	{
		template <typename ExtendedInput, typename ... InputExtensions, size_t ... Is>
		ExtendedInput make_extended_input(UpdateInput base, std::tuple<InputExtensions...> extensions, std::index_sequence<Is...>)
		{
			return ExtendedInput{base, std::move(std::get<Is>(extensions))...};
		}

		template <typename PluginLocals, typename ImplementationLocals>
		struct CRTPBaseLocals
		{
			CRTPBaseLocals(PluginLocals && p, ImplementationLocals && i)
				: plugin_locals(std::forward<PluginLocals>(p))
				, implementation_locals(std::forward<ImplementationLocals>(i))
			{}

			PluginLocals plugin_locals;
			ImplementationLocals implementation_locals;
		};
	}

	template <typename Impl, typename ... Plugins>
	void CRTPBase<Impl, Plugins...>::initialize(SDL_Window * window)
	{
		for_each_in_tuple(plugins, [window](auto & plugin) { detail::call_initialize(plugin, window); });
		detail::call_initialize_impl(implementation(), window);
	}

	template <typename Impl, typename ... Plugins>
	auto CRTPBase<Impl, Plugins...>::start_frame()
	{
		auto plugin_locals = transform_tuple(plugins, [](auto & plugin) { return detail::call_start_frame(plugin); });
		auto implementation_locals = detail::call_start_frame_impl(implementation());
		return detail::CRTPBaseLocals(std::move(plugin_locals), std::move(implementation_locals));
	}

	template <typename Impl, typename ... Plugins>
	template <typename Locals>
	void CRTPBase<Impl, Plugins...>::update(aeh::main_loop::UpdateInput input, Locals && locals)
	{
		auto input_extensions = transform_tuple(plugins, locals.plugin_locals,
			[input](auto & plugin, auto & locals) { return detail::call_update(plugin, input, locals); });

		UpdateInput extended_input = detail::make_extended_input<UpdateInput>(input, std::move(input_extensions), std::make_index_sequence<sizeof...(Plugins)>());
		detail::call_update_impl(implementation(), extended_input, locals.implementation_locals);
	}

	template <typename Impl, typename ... Plugins>
	template <typename Locals>
	void CRTPBase<Impl, Plugins...>::render(Locals && locals) const
	{
		for_each_in_tuple(plugins, [](auto const & plugin) { detail::call_pre_render(plugin); });
		detail::call_render_impl(implementation(), locals.implementation_locals);
		for_each_in_tuple_reversed(plugins, [](auto const & plugin) { detail::call_post_render(plugin); });
	}

	template <typename Impl, typename ... Plugins>
	template <typename Locals>
	void CRTPBase<Impl, Plugins...>::process_event(SDL_Event const & event, Locals && locals)
	{
		for_each_in_tuple(plugins, locals.plugin_locals,
			[&event](auto & plugin, auto & locals) { detail::call_process_event(plugin, event, locals); });
		detail::call_process_event_impl(implementation(), event, locals.implementation_locals);
	}

	template <typename Impl, typename ... Plugins>
	void CRTPBase<Impl, Plugins...>::shutdown()
	{
		detail::call_shutdown_impl(implementation());
		for_each_in_tuple_reversed(plugins, [](auto & plugin) { detail::call_shutdown(plugin); });
	}

} // namespace aeh::main_loop