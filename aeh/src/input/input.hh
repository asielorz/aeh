#pragma once

#include <bitset>
#include <array>
#include <string_view>
#include <optional>

#define binary_literal(x) 0b##x

#define XMACRO_INPUT_KEY(name, value) name = value
#define XMACRO_INPUT_MOUSE_BUTTON(name, value) name = value
#define XMACRO_INPUT_CONTROLLER_BUTTON(name, value) name = value | binary_literal(1000'0000'0000'0000)
#define XMACRO_INPUT_CONTROLLER_AXIS(name, value) name = value | binary_literal(1001'0000'0000'0000)

namespace aeh::in
{
	inline namespace enums
	{
		enum struct Key : uint16_t
		{
			#include "xm_keys.hh"
		};

		enum struct MouseButton : uint16_t
		{
			#include "xm_mouse_buttons.hh"
		};

		enum struct KeyOrMouseButton : uint16_t
		{
			#include "xm_keys.hh"
			#include "xm_mouse_buttons.hh"
		};

		enum struct ControllerButton : uint16_t
		{
			#include "xm_controller_buttons.hh"
		};

		enum struct ControllerAxis : uint16_t
		{
			#include "xm_controller_axes.hh"
		};

		enum struct ControllerInput : uint16_t
		{
			#include "xm_controller_buttons.hh"
			#include "xm_controller_axes.hh"
		};

		enum struct Controller : uint16_t
		{
			_1 = 0,
			_2 = binary_literal(0010'0000'0000'0000),
			_3 = binary_literal(0100'0000'0000'0000),
			_4 = binary_literal(0110'0000'0000'0000),
		};
	} // inline namespace enums

	enum struct BoundControllerButton : uint16_t {};
	enum struct BoundControllerAxis : uint16_t {};
	enum struct BoundControllerInput : uint16_t {};
	enum struct Control : uint16_t {};

	#undef XMACRO_INPUT_KEY
	#undef XMACRO_INPUT_MOUSE_BUTTON
	#undef XMACRO_INPUT_CONTROLLER_BUTTON
	#undef XMACRO_INPUT_CONTROLLER_AXIS

	[[nodiscard]] auto is_axis(ControllerInput input) noexcept -> bool;
	[[nodiscard]] auto is_axis(BoundControllerInput input) noexcept -> bool;
	[[nodiscard]] auto operator | (Controller controller, ControllerButton button) noexcept -> BoundControllerButton;
	[[nodiscard]] auto operator | (Controller controller, ControllerAxis axis) noexcept -> BoundControllerAxis;
	[[nodiscard]] auto operator | (Controller controller, ControllerInput control) noexcept -> BoundControllerInput;
	[[nodiscard]] auto is_controller_input(Control control) noexcept -> bool;

	[[nodiscard]] constexpr auto index_of(Key key) noexcept -> uint16_t { return static_cast<uint16_t>(key); }
	[[nodiscard]] constexpr auto index_of(MouseButton button) noexcept -> uint16_t { return static_cast<uint16_t>(button); }
	[[nodiscard]] constexpr auto index_of(KeyOrMouseButton control) noexcept -> uint16_t { return static_cast<uint16_t>(control); }
	[[nodiscard]] constexpr auto index_of(Controller controller) -> uint16_t { return static_cast<uint16_t>(controller) >> 13; }
	[[nodiscard]] constexpr auto index_of(ControllerButton button) -> uint16_t { return static_cast<uint16_t>(button) & binary_literal(0000'0000'0000'1111); }
	[[nodiscard]] constexpr auto index_of(ControllerAxis axis) -> uint16_t { return static_cast<uint16_t>(axis) & binary_literal(0000'0000'0000'1111); }
	[[nodiscard]] auto controller_button_index(BoundControllerButton button) noexcept -> uint16_t;
	[[nodiscard]] auto controller_axis_index(BoundControllerAxis axis) noexcept->std::pair<uint16_t, uint16_t>;
	[[nodiscard]] auto controller_enum_from_index(int controller_index) -> Controller;
	[[nodiscard]] auto extract_controller(BoundControllerButton button) noexcept->Controller;
	[[nodiscard]] auto extract_button(BoundControllerButton button) noexcept -> ControllerButton;
	[[nodiscard]] auto extract_controller(BoundControllerAxis button) noexcept -> Controller;
	[[nodiscard]] auto extract_axis(BoundControllerAxis button) noexcept -> ControllerAxis;

	constexpr int key_count = 186;
	constexpr int mouse_button_count = 5;
	constexpr int controller_button_count = 16;
	constexpr int controller_axis_count = 6;

	struct PreviousFrame
	{
		// Buttons
		std::bitset<key_count + mouse_button_count> buttons_down;
		std::bitset<key_count>						buttons_repeated;

		// Mouse
		int16_t mouse_pos_x = 0;
		int16_t mouse_pos_y = 0;

		// Window size
		int16_t window_width = 0;
		int16_t window_height = 0;

		// Controllers
		std::bitset<4> controllers_on = {};
		std::bitset<controller_button_count * 4> controller_buttons;
		std::array<std::array<float, controller_axis_count>, 4> controller_axes = { {{0.0f}} };
	};

	struct Frame : PreviousFrame
	{
		int mouse_wheel = 0;
		int mouse_wheel_horizontal = 0;
		std::array<char, 16> text_input = {'\0'};
	};

	struct Input
	{
		Frame current;
		PreviousFrame previous;
	};

	[[nodiscard]] auto is_down(Input const & input, Key key) noexcept -> bool;
	[[nodiscard]] auto is_down(Input const & input, MouseButton button) noexcept -> bool;
	[[nodiscard]] auto is_down(Input const & input, KeyOrMouseButton control) noexcept -> bool;
	[[nodiscard]] auto is_down(Input const & input, BoundControllerButton button) noexcept -> bool;
	[[nodiscard]] auto is_down(Input const & input, BoundControllerAxis axis, float threshold) noexcept -> bool;
	[[nodiscard]] auto is_down(Input const & input, BoundControllerInput control, float threshold) noexcept -> bool;
	[[nodiscard]] auto is_down(Input const & input, Control control, float threshold) noexcept -> bool;

	[[nodiscard]] auto is_up(Input const & input, Key key) noexcept -> bool;
	[[nodiscard]] auto is_up(Input const & input, MouseButton button) noexcept -> bool;
	[[nodiscard]] auto is_up(Input const & input, KeyOrMouseButton control) noexcept -> bool;
	[[nodiscard]] auto is_up(Input const & input, BoundControllerButton button) noexcept -> bool;
	[[nodiscard]] auto is_up(Input const & input, BoundControllerAxis axis, float threshold) noexcept -> bool;
	[[nodiscard]] auto is_up(Input const & input, BoundControllerInput control, float threshold) noexcept -> bool;
	[[nodiscard]] auto is_up(Input const & input, Control control, float threshold) noexcept -> bool;

	[[nodiscard]] auto is_pressed(Input const & input, Key key) noexcept -> bool;
	[[nodiscard]] auto is_pressed(Input const & input, MouseButton button) noexcept -> bool;
	[[nodiscard]] auto is_pressed(Input const & input, KeyOrMouseButton control) noexcept -> bool;
	[[nodiscard]] auto is_pressed(Input const & input, BoundControllerButton button) noexcept -> bool;
	[[nodiscard]] auto is_pressed(Input const & input, BoundControllerAxis axis, float threshold) noexcept -> bool;
	[[nodiscard]] auto is_pressed(Input const & input, BoundControllerInput control, float threshold) noexcept -> bool;
	[[nodiscard]] auto is_pressed(Input const & input, Control control, float threshold) noexcept -> bool;

	[[nodiscard]] auto is_released(Input const & input, Key key) noexcept -> bool;
	[[nodiscard]] auto is_released(Input const & input, MouseButton button) noexcept -> bool;
	[[nodiscard]] auto is_released(Input const & input, KeyOrMouseButton control) noexcept -> bool;
	[[nodiscard]] auto is_released(Input const & input, BoundControllerButton button) noexcept -> bool;
	[[nodiscard]] auto is_released(Input const & input, BoundControllerAxis axis, float threshold) noexcept -> bool;
	[[nodiscard]] auto is_released(Input const & input, BoundControllerInput control, float threshold) noexcept -> bool;
	[[nodiscard]] auto is_released(Input const & input, Control control, float threshold) noexcept -> bool;

	enum struct ControlState { up = 0, released = 1, pressed = 2, down = 3 };
	[[nodiscard]] auto make_control_state(bool currently_down, bool previously_down) noexcept -> ControlState;

	[[nodiscard]] auto current_state(Input const & input, Key key) noexcept -> ControlState;
	[[nodiscard]] auto current_state(Input const & input, MouseButton button) noexcept -> ControlState;
	[[nodiscard]] auto current_state(Input const & input, KeyOrMouseButton control) noexcept -> ControlState;
	[[nodiscard]] auto current_state(Input const & input, BoundControllerButton button) noexcept -> ControlState;
	[[nodiscard]] auto current_state(Input const & input, BoundControllerAxis axis, float threshold) noexcept -> ControlState;
	[[nodiscard]] auto current_state(Input const & input, BoundControllerInput control, float threshold) noexcept -> ControlState;
	[[nodiscard]] auto current_state(Input const & input, Control control, float threshold) noexcept -> ControlState;

	[[nodiscard]] auto any_key_pressed(Input const & input) noexcept -> std::optional<Key>;

	[[nodiscard]] auto is_pressed_or_repeated(Input const & input, Key key) noexcept -> bool;
	[[nodiscard]] auto axis_state(Input const & input, BoundControllerAxis axis) noexcept -> float;
	[[nodiscard]] auto axis_state(Input const & input, BoundControllerAxis axis, float dead_zone) noexcept -> float;

	[[nodiscard]] auto mouse_pos_x(Input const & input) noexcept -> int16_t;
	[[nodiscard]] auto mouse_pos_y(Input const & input) noexcept -> int16_t;
	[[nodiscard]] auto mouse_pos_prev_x(Input const& input) noexcept -> int16_t;
	[[nodiscard]] auto mouse_pos_prev_y(Input const& input) noexcept -> int16_t;
	[[nodiscard]] auto mouse_move_x(Input const & input) noexcept -> int16_t;
	[[nodiscard]] auto mouse_move_y(Input const & input) noexcept -> int16_t;
	[[nodiscard]] auto mouse_wheel(Input const & input) noexcept -> int;
	[[nodiscard]] auto mouse_wheel_horizontal(Input const & input) noexcept -> int;

	[[nodiscard]] auto window_width(Input const & input) noexcept -> int16_t;
	[[nodiscard]] auto window_height(Input const & input) noexcept -> int16_t;
	[[nodiscard]] auto window_resize_x(Input const & input) noexcept -> int16_t;
	[[nodiscard]] auto window_resize_y(Input const & input) noexcept -> int16_t;
	[[nodiscard]] auto window_resized(Input const & input) noexcept -> bool;

	[[nodiscard]] auto text_input(Input const & input) noexcept->std::string_view;
	[[nodiscard]] auto is_connected(Input const & input, Controller controller) noexcept -> bool;
	[[nodiscard]] auto just_connected(Input const & input, Controller controller) noexcept -> bool;
	[[nodiscard]] auto just_disconnected(Input const & input, Controller controller) noexcept -> bool;

	[[nodiscard]] auto update(Input const & previous_input, Frame const & current_frame) noexcept->Input;

	auto compress(Frame const & frame, PreviousFrame const & previous_frame, void * buffer, int buffer_size) noexcept -> int; // Returns the number of bytes written
	[[nodiscard]] auto decompress(PreviousFrame const & previous_frame, void const * buffer) noexcept -> Frame;

	[[nodiscard]] auto operator == (PreviousFrame const & a, PreviousFrame const & b) noexcept -> bool;
	[[nodiscard]] auto operator != (PreviousFrame const & a, PreviousFrame const & b) noexcept -> bool;
	[[nodiscard]] auto operator == (Frame const & a, Frame const & b) noexcept -> bool;
	[[nodiscard]] auto operator != (Frame const & a, Frame const & b) noexcept -> bool;
	[[nodiscard]] auto operator == (Input const & a, Input const & b) noexcept -> bool;
	[[nodiscard]] auto operator != (Input const & a, Input const & b) noexcept -> bool;

	// Avoid comparing a FrameInput with a PreviousFrameInput (by default it will slice
	// the FrameInput into a PreviusFrameInput but that's not probably what you want).
	auto operator == (Frame const & a, PreviousFrame const & b) noexcept -> bool = delete;
	auto operator == (PreviousFrame const & a, Frame const & b) noexcept -> bool = delete;
	auto operator != (Frame const & a, PreviousFrame const & b) noexcept -> bool = delete;
	auto operator != (PreviousFrame const & a, Frame const & b) noexcept -> bool = delete;

} // namespace aeh::in

#undef XMACRO_INPUT_KEY
#undef XMACRO_INPUT_MOUSE_BUTTON
#undef XMACRO_INPUT_CONTROLLER_BUTTON
#undef XMACRO_INPUT_CONTROLLER_AXIS
