#pragma once

#include <algorithm>

namespace aeh
{

	template <typename C, typename T>
	constexpr size_t erase(C & c, T value);

	template <typename C, typename Pred>
	constexpr size_t erase_if(C & c, Pred predicate);

	template <typename C, typename Pred>
	constexpr size_t update_then_erase_if(C & c, Pred predicate);

} // namespace aeh

#include "algorithm.inl"
