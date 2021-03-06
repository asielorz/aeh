#pragma once

#include "debug/assert.hh"
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

	private:
		std::function<T(int)> subscript;
		int size_;
	};

} // namespace aeh
