namespace aeh
{

	template <std::invocable<> F>
	constexpr generator_iterator<F>::generator_iterator(F const & function) noexcept(is_noexcept && std::is_nothrow_copy_constructible_v<F>)
		: generator_function(function)
		, last_generated(std::invoke(generator_function))
	{}

	template <std::invocable<> F>
	constexpr generator_iterator<F>::generator_iterator(F && function) noexcept(is_noexcept)
		: generator_function(std::move(function))
		, last_generated(std::invoke(generator_function))
	{}

	template <std::invocable<> F>
	constexpr auto generator_iterator<F>::operator * () noexcept -> value_type &
	{
		return *last_generated; 
	}

	template <std::invocable<> F>
	constexpr auto generator_iterator<F>::operator -> () noexcept -> value_type *
	{
		return &*last_generated; 
	}

	template <std::invocable<> F>
	constexpr auto generator_iterator<F>::operator == (generator_sentinel_t) const noexcept -> bool
	{
		return !last_generated.has_value(); 
	}

	template <std::invocable<> F>
	constexpr auto generator_iterator<F>::operator ++ () noexcept(is_noexcept) -> generator_iterator &
	{
		last_generated = generator_function(); 
		return *this; 
	}

	template <std::invocable<> F>
	constexpr auto generator_iterator<F>::operator ++ (int) noexcept(is_noexcept) -> generator_iterator
	{
		auto copy = *this; 
		last_generated = generator_function();
		return copy; 
	}

	//**************************************************************************************************************

	template <std::invocable<> F>
	constexpr generator<F>::generator(F const & function) noexcept(std::is_nothrow_copy_constructible_v<F>)
		: generator_function_(std::move(function))
	{}

	template <std::invocable<> F>
	constexpr generator<F>::generator(F && function) noexcept
		: generator_function_(std::move(function))
	{}

	template <std::invocable<> F>
	constexpr auto generator<F>::begin() const & noexcept(std::is_nothrow_copy_constructible_v<F>) -> generator_iterator<F>
	{
		return generator_iterator(generator_function_);
	}

	template <std::invocable<> F>
	constexpr auto generator<F>::begin() && noexcept -> generator_iterator<F>
	{
		return generator_iterator(std::move(generator_function_));
	}

	template <std::invocable<> F>
	constexpr auto generator<F>::end() const noexcept -> generator_sentinel_t
	{ 
		return generator_sentinel; 
	}

	template <std::invocable<> F>
	constexpr auto generator<F>::operator () () noexcept(is_noexcept) -> std::optional<value_type>
	{
		return std::invoke(generator_function_);
	}

	template <std::invocable<> F>
	constexpr auto generator<F>::generator_function() const & noexcept -> F const &
	{
		return generator_function_;
	}

	template <std::invocable<> F>
	constexpr auto generator<F>::generator_function() && noexcept -> F &&
	{
		return std::move(generator_function_);
	}

} // namespace aeh
