#pragma once

#include <cstdint>

namespace aeh::in
{
	inline namespace enums 
	{
		enum struct Key : uint16_t;

		enum struct Modifier
		{
			none = 0, ctrl = 1, shift = 2, alt = 4, windows = 8
		};
	} // inline namespace enums

	struct Input;

	[[nodiscard]] auto operator | (Modifier a, Modifier b) noexcept -> Modifier;
	[[nodiscard]] auto operator & (Modifier a, Modifier b) noexcept -> bool;

	//! True if the requested modifiers are down and the modifiers not requested are not up.
	//! Example: is_modifier_on(ctrl) requires ctrl to be down, shift to be up and alt to be up.
	[[nodiscard]] auto is_modifier_down(Input const & input, Modifier mod) noexcept -> bool;

	//! Same as is_pressed(input, key) && is_modifier_down(input, mod).
	[[nodiscard]] auto is_pressed(Input const& input, Key key, Modifier mod) noexcept -> bool;
	//! Same as is_pressed_or_repeated(input, key) && is_modifier_down(input, mod).
	[[nodiscard]] auto is_pressed_or_repeated(Input const & input, Key key, Modifier mod) noexcept -> bool;
	//! Same as is_down(input, key) && is_modifier_down(input, mod).
	//! Usually to be used with Modifier::none to ensure that the action does not happen when modifiers are around.
	[[nodiscard]] auto is_down(Input const& input, Key key, Modifier mod) noexcept -> bool;

} // namespace aeh::in
