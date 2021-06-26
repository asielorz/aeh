#pragma once

#include "bit.hh"
#include <cstdint>
#include <cstring>


namespace aeh
{

	// Storage type for 16 bit IEEE754 floating point numbers. Since this is emulating 16 bit floats and
	// not implementing their operations, consider using them as a storage type for passing 16 bit float
	// data to hardware accelerators or for serialization, not as a working type.
	struct half
	{
		constexpr half() noexcept = default;

#if AEH_HAS_STD_BIT_CAST
		constexpr
#endif
			static auto from_float(float value) noexcept -> half
		{
			uint32_t const float_state = bit_cast<uint32_t>(value);
			uint16_t const half_state =
				((float_state >> 16) & 0b1000'0000'0000'0000) |																								// Sign
				((((float_state & 0b0111'1111'1000'0000'0000'0000'0000'0000) - 0b0011'1000'0000'0000'0000'0000'0000'0000) >> 13) & 0b0111'1100'0000'0000) |	// Exponent
				((float_state >> 13) & 0b0011'1111'1111);																									// Mantissa

			return half::with_state(half_state);
		}
		constexpr static auto with_state(uint16_t state) noexcept -> half
		{
			half h;
			h.state = state;
			return h;
		}

#if AEH_HAS_STD_BIT
		constexpr
#endif
		auto as_float() const noexcept -> float
		{
			uint32_t const float_state =
				((state & 0b1000'0000'0000'0000) << 16) |									// Sign
				(((state & 0b0111'1100'0000'0000) + 0b0001'1100'0000'0000'0000) << 13) |	// Exponent
				((state & 0b0011'1111'1111) << 13);											// Mantissa

			return bit_cast<float>(float_state);
		}

		uint16_t state = 0;
	};

	constexpr auto operator == (half a, half b) noexcept -> bool { return a.state == b.state; }
	constexpr auto operator != (half a, half b) noexcept -> bool { return !(a == b); }

} // namespace aeh
