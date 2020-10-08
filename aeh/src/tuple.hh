#pragma once

#include <tuple>

namespace aeh
{

	template <typename Tuple, typename F>
	void for_each_in_tuple(Tuple && tuple, F && function);

	template <typename Tuple, typename F>
	void for_each_in_tuple_reversed(Tuple && tuple, F && function);

	template <typename Tuple, typename F>
	auto transform_tuple(Tuple && tuple, F && function);

}

#include "tuple.inl"
