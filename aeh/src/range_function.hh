#pragma once

#include "debug/assert.hh"
#include "concepts.hh"
#include <functional>

namespace aeh
{

	template <typename T>
	struct range_function
	{
		template <std::convertible_to<std::function<T(int)>> F>
		constexpr range_function(F const & subscript_, int size__) noexcept
			: subscript(subscript_)
			, size_(size__)
		{}

		[[nodiscard]] constexpr auto size() const noexcept -> int { return size_; }
		[[nodiscard]] constexpr auto operator[] (int i) const noexcept -> T { debug_assert(i >= 0 && i < size()); return subscript(i); }

		template <typename U, aeh::invocable_r<T, U> F>
		[[nodiscard]] static constexpr auto from_span(std::span<U const> span, F && f)
		{
			return range_function([span, f](int i) -> T { return f(span[static_cast<size_t>(i)]); }, static_cast<int>(span.size()));
		}

	private:
		std::function<T(int)> subscript;
		int size_;
	};

} // namespace aeh
