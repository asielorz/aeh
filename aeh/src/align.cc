#include "align.hh"

namespace aeh
{

	auto add_size_aligned(int prev_size, int size_to_add, int alignment) noexcept -> int
	{
		return align(prev_size, alignment) + size_to_add;
	}

	auto is_divisible(int dividend, int divisor) noexcept -> bool
	{
		return dividend % divisor == 0;
	}

	auto is_aligned(int address, int alignment) noexcept -> bool
	{
		return is_divisible(address, alignment);
	}

} // namespace aeh
