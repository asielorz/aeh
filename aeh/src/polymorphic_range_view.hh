#pragma once

#include "function_ref.hh"
#include "debug/assert.hh"

template <typename T>
struct polymorphic_range_view
{
	template <std::convertible_to<aeh::function_ref<T(int)>> F>
	constexpr polymorphic_range_view(F const & subscript_, int size__) noexcept
		: subscript(subscript_)
		, size_(size__)
	{}

	[[nodiscard]] constexpr auto size() const noexcept -> int { return size_; }
	[[nodiscard]] constexpr auto operator[] (int i) const noexcept -> T { debug_assert(i < size()); return subscript(i); }

private:
	aeh::function_ref<T(int)> subscript;
	int size_;
};
