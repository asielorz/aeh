#pragma once

#ifdef __has_include
#	if __has_include(<version>)
#		include <version>
#	endif
#endif

#if !defined(__cpp_lib_bit_cast)
#	define AEH_HAS_STD_BIT_CAST false
#	include <cstring>
#	include <type_traits>
#else
#	define AEH_HAS_STD_BIT_CAST true
#	include <bit>
#endif

namespace aeh
{
#if AEH_HAS_STD_BIT_CAST
	using std::bit_cast;
#else
	template <typename To, typename From,
		typename = std::enable_if_t<
			std::conjunction_v<
				std::bool_constant<sizeof(To) == sizeof(From)>, std::is_trivially_copyable<To>, std::is_trivially_copyable<From>
			>
		>>
	[[nodiscard]] To bit_cast(From const& input) noexcept
	{
		To result;
		std::memcpy(&result, &input, sizeof(From));
		return result;
	}
#endif
} // namespace aeh

