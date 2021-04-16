namespace aeh
{
	namespace detail
	{
		template <typename Tuple, typename F, size_t ... Is>
		void for_each_in_tuple_impl(Tuple && tuple, F && function, std::index_sequence<Is...>)
		{
			(static_cast<void>(function(std::get<Is>(tuple))), ...);
		}

		template <typename Tuple1, typename Tuple2, typename F, size_t ... Is>
		void for_each_in_tuple_binary_impl(Tuple1 && tuple1, Tuple2 && tuple2, F && function, std::index_sequence<Is...>)
		{
			(static_cast<void>(function(std::get<Is>(tuple1), std::get<Is>(tuple2))), ...);
		}

		template <typename Tuple, typename F, size_t ... Is>
		void for_each_in_tuple_reversed_impl(Tuple && tuple, F && function, std::index_sequence<Is...>)
		{
			size_t const last = std::tuple_size_v<std::decay_t<Tuple>> -1;
			(static_cast<void>(function(std::get<last - Is>(tuple))), ...);
		}

		template <typename Tuple, typename F, size_t ... Is>
		auto transform_tuple_unary_impl(Tuple && tuple, F && function, std::index_sequence<Is...>)
		{
			return std::make_tuple(function(std::get<Is>(tuple))...);
		}

		template <typename Tuple1, typename Tuple2, typename F, size_t ... Is>
		auto transform_tuple_binary_impl(Tuple1 && tuple1, Tuple2 && tuple2, F && function, std::index_sequence<Is...>)
		{
			return std::make_tuple(function(std::get<Is>(tuple1), std::get<Is>(tuple2))...);
		}

		template <typename Tuple, typename F, size_t ... Is>
		decltype(auto) tuple_at_impl(Tuple && tuple, size_t i, F && function, std::index_sequence<Is...>)
		{
			using return_type = decltype(std::invoke(std::forward<F>(function), std::get<0>(std::forward<Tuple>(tuple))));
			static constexpr function_ptr<return_type(Tuple &&, F&&)> jump_table[] = { 
				[](Tuple && tuple, F && function) { return std::invoke(std::forward<F>(function), std::get<Is>(std::forward<Tuple>(tuple))); }... 
			};
			return jump_table[i](std::forward<Tuple>(tuple), std::forward<F>(function));
		}

	} // namespace detail

	template <typename Tuple, typename F>
	void for_each_in_tuple(Tuple && tuple, F && function)
	{
		detail::for_each_in_tuple_impl(std::forward<Tuple>(tuple), function, std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>());
	}

	template <typename Tuple1, typename Tuple2, typename F>
	void for_each_in_tuple(Tuple1 && tuple1, Tuple2 && tuple2, F && function)
	{
		detail::for_each_in_tuple_binary_impl(std::forward<Tuple1>(tuple1), std::forward<Tuple2>(tuple2), function,
			std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple1>>>());
	}

	template <typename Tuple, typename F>
	void for_each_in_tuple_reversed(Tuple && tuple, F && function)
	{
		detail::for_each_in_tuple_reversed_impl(std::forward<Tuple>(tuple), function, std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>());
	}

	template <typename Tuple, typename F>
	auto transform_tuple(Tuple && tuple, F && function)
	{
		return detail::transform_tuple_unary_impl(std::forward<Tuple>(tuple), std::forward<F>(function), 
			std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>());
	}

	template <typename Tuple1, typename Tuple2, typename F>
	auto transform_tuple(Tuple1 && tuple1, Tuple2 && tuple2, F && function)
	{
		static_assert(std::tuple_size_v<std::decay_t<Tuple1>> == std::tuple_size_v<std::decay_t<Tuple2>>, 
			"Both tuples for binary transform_tuple must have the same size.");

		return detail::transform_tuple_binary_impl(std::forward<Tuple1>(tuple1), std::forward<Tuple2>(tuple2), std::forward<F>(function), 
			std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple1>>>());
	}

	template <typename Tuple, typename F>
	decltype(auto) tuple_at(Tuple && tuple, size_t i, F && function)
	{
		return detail::tuple_at_impl(std::forward<Tuple>(tuple), i, std::forward<F>(function), std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>());
	}

} // namespace aeh
