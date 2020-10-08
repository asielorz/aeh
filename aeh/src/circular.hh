#pragma once

namespace aeh
{

	template <typename T>
	T increment_circular(T x, T first, T last)
	{
		++x;
		if (x == last)
			return first;
		else
			return x;
	}

	template <typename T>
	T decrement_circular(T x, T first, T last)
	{
		if (x == first)
			return --last;
		else
			return --x;
	}

} // namespace aeh
