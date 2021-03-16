#pragma once

#include <algorithm>
#include <numeric>

namespace aeh
{

	template <typename C, typename T>
	constexpr size_t erase(C & c, T value);

	template <typename C, typename Pred>
	constexpr size_t erase_if(C & c, Pred predicate);

	template <typename C, typename Pred>
	constexpr size_t update_then_erase_if(C & c, Pred predicate);

	template <typename It> requires std::forward_iterator<It> && requires(It a) { *a + *a; }
	constexpr auto sum(It begin, It end) noexcept -> std::decay_t<decltype(*begin)>;

	//template <typename R>
	//constexpr auto sum(R && range) noexcept -> decltype(sum(std::begin(range), std::end(range)));

} // namespace aeh

#include "algorithm.inl"
