#pragma once

#include <type_traits>

namespace aeh
{
	template <typename T>
	concept is_trivially_copyable = std::is_trivially_copyable_v<T>;
} // namespace aeh
