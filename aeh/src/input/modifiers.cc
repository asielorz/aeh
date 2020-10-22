#include "modifiers.hh"
#include "input.hh"

namespace aeh::in
{

	auto operator | (Modifier a, Modifier b) noexcept -> Modifier 
	{
		return static_cast<Modifier>(static_cast<int>(a) | static_cast<int>(b)); 
	}

	auto operator & (Modifier a, Modifier b) noexcept -> bool
	{
		return static_cast<bool>(static_cast<int>(a) & static_cast<int>(b));
	}

	auto is_modifier_down(Input const & input, Modifier mod) noexcept -> bool
	{
		bool const need_ctrl = mod & Modifier::ctrl;
		bool const need_shift = mod & Modifier::shift;
		bool const need_alt = mod & Modifier::alt;
		bool const need_windows = mod & Modifier::windows;

		return
			((is_down(input, Key::left_control) || is_down(input, Key::right_control)) == need_ctrl) &&
			((is_down(input, Key::left_shift) || is_down(input, Key::right_shift)) == need_shift) &&
			((is_down(input, Key::left_alt) || is_down(input, Key::right_alt)) == need_alt) &&
			((is_down(input, Key::left_windows) || is_down(input, Key::right_windows)) == need_windows);
	}

	auto is_pressed(Input const & input, Key key, Modifier mod) noexcept -> bool
	{
		return is_pressed(input, key) && is_modifier_down(input, mod);
	}

	auto is_pressed_or_repeated(Input const& input, Key key, Modifier mod) noexcept -> bool
	{
		return is_pressed_or_repeated(input, key) && is_modifier_down(input, mod);
	}

	auto is_down(Input const& input, Key key, Modifier mod) noexcept -> bool
	{
		return is_down(input, key) && is_modifier_down(input, mod);
	}

} // namespace aeh::in
