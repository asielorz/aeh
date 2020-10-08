#pragma once

#include <cstddef>

namespace aeh
{

	template <typename T>
	constexpr auto align(T p, T alignment) noexcept -> T
	{
		return ((p - 1) | (alignment - 1)) + 1;
	}

	auto add_size_aligned(int prev_size, int size_to_add, int alignment) noexcept -> int;

	template <typename T>
	constexpr auto align(T * address, size_t alignment) noexcept -> T *
	{
		return reinterpret_cast<T *>(align(reinterpret_cast<size_t>(address), alignment));
	}

	auto is_divisible(int dividend, int divisor) noexcept -> bool;
	auto is_aligned(int address, int alignment) noexcept -> bool;

} // namespace aeh
