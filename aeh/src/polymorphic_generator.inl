namespace aeh
{

	template <typename T>
	template <std::convertible_to<std::function<std::optional<T>()>> F>
	polymorphic_generator<T>::polymorphic_generator(F f)
		: generator<std::function<std::optional<T>()>>(std::move(f))
	{}

	template <typename T>
	template <std::convertible_to<std::function<std::optional<T>()>> F>
	polymorphic_generator<T>::polymorphic_generator(generator<F> g)
		: generator<std::function<std::optional<T>()>>(std::move(g).generator_function())
	{}

	template <typename F>
	auto make_polymorphic(generator<F> g) -> polymorphic_generator<typename generator<F>::value_type>
	{
		return polymorphic_generator<typename generator<F>::value_type>(std::move(g));
	}

} // namespace aeh
