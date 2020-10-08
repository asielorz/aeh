#pragma once

namespace aeh
{
	template <unsigned N> struct priority_tag : priority_tag<N - 1> {};
	template <> struct priority_tag<0> {};
	template <unsigned N> constexpr priority_tag<N> priority_tag_v;
}
