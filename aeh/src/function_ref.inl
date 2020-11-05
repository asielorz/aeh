namespace aeh
{

	template <typename Caller>
	constexpr auto detail::function_ref_base<Caller>::has_value() const noexcept -> bool
	{
		return caller != nullptr;
	}

	template <typename Caller>
	constexpr detail::function_ref_base<Caller>::operator bool() const noexcept
	{
		return has_value();
	}

	template <typename Ret, typename ... Args>
	template <typename Callable, typename>
	constexpr function_ref<Ret(Args...)>::function_ref(Callable & c) noexcept
		: Base(
			(void *)std::addressof(c),
			[](void * context, Args ... args) -> Ret { return std::invoke(*static_cast<Callable *>(context), args...); }
		)
	{}

	template <typename Ret, typename ... Args>
	template <typename Callable, typename>
	constexpr function_ref<Ret(Args...)>::function_ref(Callable const & c) noexcept
		: Base(
			(void *)std::addressof(c),
			[](void * context, Args ... args) -> Ret { return std::invoke(*static_cast<Callable const *>(context), args...); }
		)
	{}

	template <typename Ret, typename ... Args>
	constexpr Ret function_ref<Ret(Args...)>::operator () (Args ... args) const
	{
		return this->caller(this->context, args...);
	}

	template <typename Ret, typename ... Args>
	template <typename Callable, typename>
	constexpr function_ref<Ret(Args...) noexcept>::function_ref(Callable & c) noexcept
		: Base(
			(void *)std::addressof(c),
			[](void * context, Args ... args) noexcept -> Ret { return std::invoke(*static_cast<Callable *>(context), args...); }
		)
	{}

	template <typename Ret, typename ... Args>
	constexpr Ret function_ref<Ret(Args...) noexcept>::operator () (Args ... args) const noexcept
	{
		return this->caller(this->context, args...);
	}

	template <typename Caller>
	constexpr auto detail::function_ref_base<Caller>::operator == (function_ref_base other) const noexcept -> bool
	{
		return context == other.context && caller == other.caller;
	}

	template <typename Caller>
	constexpr auto detail::function_ref_base<Caller>::operator != (function_ref_base other) const noexcept -> bool
	{
		return !(*this == other);
	}

	template <typename F>
	constexpr auto operator == (function_ref<F> f, std::nullptr_t) noexcept -> bool
	{
		return !f.has_value();
	}

	template <typename F>
	constexpr auto operator != (function_ref<F> f, std::nullptr_t) noexcept -> bool
	{
		return f.has_value();
	}

	template <typename F>
	constexpr auto operator == (std::nullptr_t, function_ref<F> f) noexcept -> bool
	{
		return !f.has_value();
	}

	template <typename F>
	constexpr auto operator != (std::nullptr_t, function_ref<F> f) noexcept -> bool
	{
		return f.has_value();
	}

} // namespace aeh
