#pragma once

#include <type_traits>
#include <concepts>

namespace aeh
{
	template <typename T>
	concept is_trivially_copyable = std::is_trivially_copyable_v<T>;

	// Invocable and returns the given type.
	template <typename F, typename R, typename ... Args>
	concept invocable_r = std::convertible_to<std::invoke_result_t<F, Args...>, R> && std::invocable<F, Args...>;
} // namespace aeh
