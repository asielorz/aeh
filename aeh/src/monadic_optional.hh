#pragma once

#include <optional>

// TODO: Expand this to support returning an optional and forming an actual functor or monad when someone needs it.
// Until then I just want to chain it to assign to. Too long chains of functors and monads in C++ are usually unreadable anyway.
template <typename T, std::invocable<T> F>
auto operator >> (std::optional<T> maybe_t, F next) -> void
{
	if (maybe_t)
		next(std::move(*maybe_t));
}

namespace aeh
{

	template <typename T>
	struct assign_to
	{
		explicit assign_to(T & var) noexcept : variable_to_assign(std::addressof(var)) {}

		auto operator() (T const & t) const noexcept -> void { *variable_to_assign = t; }
		auto operator() (T && t) const noexcept -> void { *variable_to_assign = std::move(t); }

	private:
		T * variable_to_assign;
	};

} // namespace aeh
