#pragma once

#include "function_ptr.hh"
#include <tuple>
#include <functional>

namespace aeh
{

	template <typename Tuple, typename F>
	void for_each_in_tuple(Tuple && tuple, F && function);

	template <typename Tuple1, typename Tuple2, typename F>
	void for_each_in_tuple(Tuple1 && tuple1, Tuple2 && tuple2, F && function);

	template <typename Tuple, typename F>
	void for_each_in_tuple_reversed(Tuple && tuple, F && function);

	template <typename Tuple, typename F>
	auto transform_tuple(Tuple && tuple, F && function);

	template <typename Tuple1, typename Tuple2, typename F>
	auto transform_tuple(Tuple1 && tuple1, Tuple2 && tuple2, F && function);

	template <typename Tuple, typename F>
	decltype(auto) tuple_at(Tuple && tuple, size_t i, F && function);

} // namespace aeh

#include "tuple.inl"
