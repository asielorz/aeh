#pragma once

#include "generator.hh"
#include <functional>

namespace aeh
{

	template <typename T>
	struct polymorphic_generator : public generator<std::function<std::optional<T>()>>
	{
		template <std::convertible_to<std::function<std::optional<T>()>> F>
		explicit polymorphic_generator(F f);

		template <std::convertible_to<std::function<std::optional<T>()>> F>
		polymorphic_generator(generator<F> f);
	};

	template <typename F>
	[[nodiscard]] auto make_polymorphic(generator<F> g) -> polymorphic_generator<typename generator<F>::value_type>;

} // namespace aeh

#include "polymorphic_generator.inl"
