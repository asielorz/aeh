#include "input.hh"
#include "debug/unreachable.hh"
#include "binary_io.hh"

namespace aeh::in
{
	namespace
	{
		constexpr uint16_t controller_button_mask = binary_literal(1000'0000'0000'0000);
		constexpr uint16_t controller_axis_mask = binary_literal(1001'0000'0000'0000);
		constexpr uint16_t controller_mask = binary_literal(0110'0000'0000'0000);
		constexpr uint16_t index_mask = binary_literal(0000'0000'0000'1111);
		constexpr uint16_t controller_input_is_axis_mask = binary_literal(0001'0000'0000'0000);
		constexpr uint16_t control_is_controller_input_mask = binary_literal(0001'0000'0000'0000);

		auto controller_index(Controller controller) noexcept -> uint16_t
		{
			return static_cast<uint16_t>(controller) >> 13;
		}
	} // namespace

	auto controller_button_index(BoundControllerButton button) noexcept -> uint16_t
	{
		uint16_t const index = static_cast<uint16_t>(button);
		uint16_t const controller_index = (index & controller_mask) >> 13;
		uint16_t const button_index = index & index_mask;
		return controller_button_count * controller_index + button_index;
	}

	auto controller_axis_index(BoundControllerAxis axis) noexcept -> std::pair<uint16_t, uint16_t>
	{
		uint16_t const index = static_cast<uint16_t>(axis);
		uint16_t const controller_index = (index & controller_mask) >> 13;
		uint16_t const axis_index = index & index_mask;
		return {controller_index, axis_index};
	}

	auto controller_enum_from_index(int controller_index) -> Controller
	{
		switch (controller_index)
		{
			case 0: return Controller::_1;
			case 1: return Controller::_2;
			case 2: return Controller::_3;
			case 3: return Controller::_4;
		}
		debug::declare_unreachable();
	}

	auto extract_controller(BoundControllerButton button) noexcept -> Controller
	{
		return static_cast<Controller>(static_cast<uint16_t>(button) & controller_mask);
	}

	auto extract_button(BoundControllerButton button) noexcept -> ControllerButton
	{
		return static_cast<ControllerButton>(static_cast<uint16_t>(button) & ~controller_mask);
	}

	auto extract_controller(BoundControllerAxis axis) noexcept -> Controller
	{
		return static_cast<Controller>(static_cast<uint16_t>(axis) & controller_mask);
	}

	auto extract_axis(BoundControllerAxis axis) noexcept -> ControllerAxis
	{
		return static_cast<ControllerAxis>(static_cast<uint16_t>(axis) & ~controller_mask);
	}

	auto is_axis(ControllerInput input) noexcept -> bool
	{
		return static_cast<uint16_t>(input) & controller_input_is_axis_mask;
	}

	auto is_axis(BoundControllerInput input) noexcept -> bool
	{
		return static_cast<uint16_t>(input) & controller_input_is_axis_mask;
	}

	auto operator | (Controller controller, ControllerButton button) noexcept -> BoundControllerButton
	{
		return static_cast<BoundControllerButton>(static_cast<uint16_t>(controller) | static_cast<uint16_t>(button));
	}

	auto operator | (Controller controller, ControllerAxis axis) noexcept -> BoundControllerAxis
	{
		return static_cast<BoundControllerAxis>(static_cast<uint16_t>(controller) | static_cast<uint16_t>(axis));
	}

	auto operator | (Controller controller, ControllerInput control) noexcept -> BoundControllerInput
	{
		return static_cast<BoundControllerInput>(static_cast<uint16_t>(controller) | static_cast<uint16_t>(control));
	}

	auto is_controller_input(Control control) noexcept -> bool
	{
		return static_cast<uint16_t>(control) & control_is_controller_input_mask;
	}

	//*****************************************************************************************************************

#define IMPL_FOR_KEY(function_name)																				\
	auto function_name(Input const & input, Key key) noexcept -> bool											\
	{																											\
		return function_name(input, static_cast<KeyOrMouseButton>(key));										\
	}

#define IMPL_FOR_MOUSE_BUTTON(function_name)																	\
	auto function_name(Input const & input, MouseButton button) noexcept -> bool								\
	{																											\
		return function_name(input, static_cast<KeyOrMouseButton>(button));										\
	}

#define IMPL_FOR_CONTROLLER_INPUT(function_name)																\
	auto function_name(Input const & input, BoundControllerInput control, float threshold) noexcept -> bool		\
	{																											\
		if (is_axis(control))																					\
			return function_name(input, static_cast<BoundControllerAxis>(control), threshold);					\
		else																									\
			return function_name(input, static_cast<BoundControllerButton>(control));							\
	}

#define IMPL_FOR_CONTROL(function_name)																			\
	auto function_name(Input const & input, Control control, float threshold) noexcept -> bool					\
	{																											\
		if (is_controller_input(control))																		\
			return function_name(input, static_cast<BoundControllerInput>(control), threshold);					\
		else																									\
			return function_name(input, static_cast<KeyOrMouseButton>(control));								\
	}

	IMPL_FOR_KEY(is_down);
	IMPL_FOR_KEY(is_up);
	IMPL_FOR_KEY(is_pressed);
	IMPL_FOR_KEY(is_released);

	IMPL_FOR_MOUSE_BUTTON(is_down);
	IMPL_FOR_MOUSE_BUTTON(is_up);
	IMPL_FOR_MOUSE_BUTTON(is_pressed);
	IMPL_FOR_MOUSE_BUTTON(is_released);

	IMPL_FOR_CONTROLLER_INPUT(is_down);
	IMPL_FOR_CONTROLLER_INPUT(is_up);
	IMPL_FOR_CONTROLLER_INPUT(is_pressed);
	IMPL_FOR_CONTROLLER_INPUT(is_released);

	IMPL_FOR_CONTROL(is_down);
	IMPL_FOR_CONTROL(is_up);
	IMPL_FOR_CONTROL(is_pressed);
	IMPL_FOR_CONTROL(is_released);

	auto is_down(Input const & input, KeyOrMouseButton control) noexcept -> bool
	{
		return input.current.buttons_down[static_cast<uint16_t>(control)];
	}

	auto is_down(Input const & input, BoundControllerButton button) noexcept -> bool
	{
		return input.current.controller_buttons[controller_button_index(button)];
	}

	auto is_down(Input const & input, BoundControllerAxis axis, float threshold) noexcept -> bool
	{
		auto const [controller_index, axis_index] = controller_axis_index(axis);
		if (threshold >= 0)
			return input.current.controller_axes[controller_index][axis_index] >= threshold;
		else
			return input.current.controller_axes[controller_index][axis_index] < threshold;
	}

	auto is_up(Input const & input, KeyOrMouseButton control) noexcept -> bool
	{
		return !is_down(input, control);
	}

	auto is_up(Input const & input, BoundControllerButton button) noexcept -> bool
	{
		return !is_down(input, button);
	}

	auto is_up(Input const & input, BoundControllerAxis axis, float threshold) noexcept -> bool
	{
		return !is_down(input, axis, threshold);
	}

	auto is_pressed(Input const & input, KeyOrMouseButton control) noexcept -> bool
	{
		return is_down(input, control) && !input.previous.buttons_down[static_cast<uint16_t>(control)];
	}

	auto is_pressed(Input const & input, BoundControllerButton button) noexcept -> bool
	{
		auto const index = controller_button_index(button);
		return input.current.controller_buttons[index] && !input.previous.controller_buttons[index];
	}

	auto is_pressed(Input const & input, BoundControllerAxis axis, float threshold) noexcept -> bool
	{
		auto const [controller_index, axis_index] = controller_axis_index(axis);

		if (threshold >= 0)
			return input.current.controller_axes[controller_index][axis_index] >= threshold &&
				   input.previous.controller_axes[controller_index][axis_index] < threshold;
		else
			return input.current.controller_axes[controller_index][axis_index] <= threshold &&
				   input.previous.controller_axes[controller_index][axis_index] > threshold;
	}

	auto is_released(Input const & input, KeyOrMouseButton control) noexcept -> bool
	{
		return is_up(input, control) && input.previous.buttons_down[static_cast<uint16_t>(control)];
	}

	auto is_released(Input const & input, BoundControllerButton button) noexcept -> bool
	{
		auto const index = controller_button_index(button);
		return !input.current.controller_buttons[index] && input.previous.controller_buttons[index];
	}

	auto is_released(Input const & input, BoundControllerAxis axis, float threshold) noexcept -> bool
	{
		auto const [controller_index, axis_index] = controller_axis_index(axis);

		if (threshold >= 0)
			return input.current.controller_axes[controller_index][axis_index] < threshold &&
				   input.previous.controller_axes[controller_index][axis_index] >= threshold;
		else
			return input.current.controller_axes[controller_index][axis_index] > threshold &&
				   input.previous.controller_axes[controller_index][axis_index] <= threshold;
	}

	auto is_pressed_or_repeated(Input const & input, Key key) noexcept -> bool
	{
		return is_pressed(input, key) || input.current.buttons_repeated[static_cast<uint16_t>(key)];
	}

	auto axis_state(Input const & input, BoundControllerAxis axis) noexcept -> float
	{
		auto const [controller_index, axis_index] = controller_axis_index(axis);
		return input.current.controller_axes[controller_index][axis_index];
	}

	auto axis_state(Input const & input, BoundControllerAxis axis, float dead_zone) noexcept -> float
	{
		float const state = axis_state(input, axis);
		if (abs(state) < dead_zone)
			return 0.0f;
		else
			return state;
	}

	auto mouse_pos_x(Input const & input) noexcept -> int16_t
	{
		return input.current.mouse_pos_x;
	}

	auto mouse_pos_y(Input const & input) noexcept -> int16_t
	{
		return input.current.mouse_pos_y;
	}

	auto mouse_pos_prev_x(Input const& input) noexcept -> int16_t
	{
		return input.previous.mouse_pos_x;
	}

	auto mouse_pos_prev_y(Input const& input) noexcept -> int16_t
	{
		return input.previous.mouse_pos_y;
	}

	auto mouse_move_x(Input const & input) noexcept -> int16_t
	{
		return input.current.mouse_pos_x - input.previous.mouse_pos_x;
	}

	auto mouse_move_y(Input const & input) noexcept -> int16_t
	{
		return input.current.mouse_pos_y - input.previous.mouse_pos_y;
	}

	auto text_input(Input const & input) noexcept -> std::string_view
	{
		return input.current.text_input.data();
	}

	auto mouse_wheel(Input const & input) noexcept -> int
	{
		return input.current.mouse_wheel;
	}

	auto mouse_wheel_horizontal(Input const & input) noexcept -> int
	{
		return input.current.mouse_wheel_horizontal;
	}

	auto window_width(Input const & input) noexcept -> int16_t
	{
		return input.current.window_width;
	}

	auto window_height(Input const & input) noexcept -> int16_t
	{
		return input.current.window_height;
	}

	auto window_resize_x(Input const & input) noexcept -> int16_t
	{
		return input.current.window_width - input.previous.window_width;
	}

	auto window_resize_y(Input const & input) noexcept -> int16_t
	{
		return input.current.window_height - input.previous.window_height;
	}

	auto window_resized(Input const & input) noexcept -> bool
	{
		return (window_resize_x(input) != 0) || (window_resize_y(input) != 0);
	}

	auto is_connected(Input const & input, Controller controller) noexcept -> bool
	{
		return input.current.controllers_on[controller_index(controller)];
	}

	auto just_connected(Input const & input, Controller controller) noexcept -> bool
	{
		auto const index = controller_index(controller);
		return input.current.controllers_on[index] && !input.previous.controllers_on[index];
	}

	auto just_disconnected(Input const & input, Controller controller) noexcept -> bool
	{
		auto const index = controller_index(controller);
		return !input.current.controllers_on[index] && input.previous.controllers_on[index];
	}

	auto update(Input const & previous_input, Frame const & current_frame) noexcept -> Input
	{
		Input new_input;
		new_input.current = current_frame;
		new_input.previous = previous_input.current;
		return new_input;
	}

	struct CompressedInputFrameHeader
	{
		uint16_t equal_frames : 7;
		uint16_t input_count : 9;
	};
	static_assert(sizeof(CompressedInputFrameHeader) == 2);
	static_assert(alignof(CompressedInputFrameHeader) == 2);

	constexpr uint16_t mouse_pos_id = mouse_button_count;
	constexpr uint16_t mouse_wheel_id = mouse_pos_id + 1;
	constexpr uint16_t text_input_id = mouse_pos_id + 2;
	constexpr uint16_t controllers_on_id = mouse_pos_id + 3;

	constexpr uint16_t key_repeat_mask = binary_literal(0100'0000'0000'0000);

	struct PackedMouseWheel
	{
		int8_t mouse_wheel : 4;
		int8_t mouse_wheel_horizontal : 4;
	};
	static_assert(sizeof(PackedMouseWheel) == 1);
	static_assert(alignof(PackedMouseWheel) == 1);

	auto compress(Frame const & frame, PreviousFrame const & previous_frame, void * buffer, int buffer_size) noexcept -> int
	{
		(void)buffer_size;

		auto * const header = reinterpret_cast<CompressedInputFrameHeader *>(buffer);
		header->equal_frames = 0;
		header->input_count = 0;

		uint16_t * it = reinterpret_cast<uint16_t *>(header + 1);

		// Keys
		for (int i = 0; i < key_count; ++i)
		{
			if (frame.buttons_down[i] != previous_frame.buttons_down[i])
			{
				*it++ = uint16_t(i);
				header->input_count++;
			}
			else if (frame.buttons_repeated[i] != previous_frame.buttons_repeated[i])
			{
				*it++ = uint16_t(i) | key_repeat_mask;
				header->input_count++;
			}
		}

		// Mouse buttons
		for (int i = key_count; i < key_count + mouse_button_count; ++i)
		{
			if (frame.buttons_down[i] != previous_frame.buttons_down[i])
			{
				*it++ = uint16_t(i);
				header->input_count++;
			}
		}

		// Controller buttons
		for (int i = 0; i < 4; ++i)
		{
			if (frame.controllers_on[i])
			{
				for (int j = 0; j < controller_button_count; ++j)
				{
					int const index = i * controller_button_count + j;
					if (frame.controller_buttons[index] != previous_frame.controller_buttons[index])
					{
						*it++ = uint16_t((i << 13) | j) | controller_button_mask;
						header->input_count++;
					}
				}
			}
		}

		// Controller axes
		for (int i = 0; i < 4; ++i)
			if (frame.controllers_on[i])
				for (int j = 0; j < controller_axis_count; ++j)
					if (frame.controller_axes[i][j] != previous_frame.controller_axes[i][j])
					{
						*it++ = uint16_t((i << 13) | j) | controller_axis_mask;
						*reinterpret_cast<float *>(it) = frame.controller_axes[i][j];
						it += 2;
						header->input_count++;
					}

		// Controllers on
		if (frame.controllers_on != previous_frame.controllers_on)
		{
			*it++ = controllers_on_id;
			uint8_t controllers_on_data = 0;
			if (frame.controllers_on[0]) controllers_on_data |= binary_literal(0000'0001);
			if (frame.controllers_on[1]) controllers_on_data |= binary_literal(0000'0010);
			if (frame.controllers_on[2]) controllers_on_data |= binary_literal(0000'0100);
			if (frame.controllers_on[3]) controllers_on_data |= binary_literal(0000'1000);
			*reinterpret_cast<uint8_t *>(it) = controllers_on_data;
			reinterpret_cast<uint8_t *&>(it)++;
			header->input_count++;
		}

		// Mouse position
		if (frame.mouse_pos_x != previous_frame.mouse_pos_x || frame.mouse_pos_y != previous_frame.mouse_pos_y)
		{
			*it++ = mouse_pos_id;
			*reinterpret_cast<int16_t *>(it) = frame.mouse_pos_x;
			++it;
			*reinterpret_cast<int16_t *>(it) = frame.mouse_pos_y;
			++it;
			header->input_count++;
		}

		// Mouse wheel
		if (frame.mouse_wheel != 0 || frame.mouse_wheel_horizontal != 0)
		{
			*it++ = mouse_wheel_id;
			PackedMouseWheel packed_wheel;
			packed_wheel.mouse_wheel = frame.mouse_wheel;
			packed_wheel.mouse_wheel_horizontal = frame.mouse_wheel_horizontal;
			write_and_advance(it, packed_wheel);
			header->input_count++;
		}

		// Text input
		if (frame.text_input[0] != '\0')
		{
			*it++ = text_input_id;
			int8_t len = 0;
			for (; len < 16; ++len)
				if (frame.text_input[len] == '\0')
					break;

			auto & byte_it = reinterpret_cast<int8_t * &>(it);
			// Write length
			*byte_it++ = len;
			// Write text input
			for (int8_t i = 0; i < len; ++i)
				* byte_it++ = frame.text_input[i];

			header->input_count++;
		}

		return static_cast<int>(reinterpret_cast<char *>(it) - reinterpret_cast<char *>(buffer));
	}

	auto decompress(PreviousFrame const & previous_frame, void const * buffer) noexcept -> Frame
	{
		Frame frame = { previous_frame };

		auto it = reinterpret_cast<uint16_t const *>(buffer);
		auto const header = *reinterpret_cast<CompressedInputFrameHeader const *>(it);
		++it;

		int const count = header.input_count;
		for (int i = 0; i < count; ++i)
		{
			uint16_t const id = *it++;

			if (id & controller_button_mask)
			{
				// Controller axis
				if (id & controller_input_is_axis_mask)
				{
					auto const [controller_index, axis_index] = controller_axis_index(static_cast<BoundControllerAxis>(id));
					float const value = read_and_advance<float>(it);
					frame.controller_axes[controller_index][axis_index] = value;
				}
				// Controller button
				else
				{
					auto const index = controller_button_index(static_cast<BoundControllerButton>(id));
					frame.controller_buttons[index].flip();
				}
			}
			// Controllers on
			else if (id == controllers_on_id)
			{
				uint8_t const controllers_on_data = read_and_advance<uint8_t>(it);
				frame.controllers_on[0] = controllers_on_data & binary_literal(0000'0001);
				frame.controllers_on[1] = controllers_on_data & binary_literal(0000'0010);
				frame.controllers_on[2] = controllers_on_data & binary_literal(0000'0100);
				frame.controllers_on[3] = controllers_on_data & binary_literal(0000'1000);
			}
			// Mouse position
			else if (id == mouse_pos_id)
			{
				frame.mouse_pos_x = read_and_advance<int16_t>(it);
				frame.mouse_pos_y = read_and_advance<int16_t>(it);
			}
			// Mouse wheel
			else if (id == mouse_wheel_id)
			{
				auto const packed_wheel = read_and_advance<PackedMouseWheel>(it);
				frame.mouse_wheel = packed_wheel.mouse_wheel;
				frame.mouse_wheel_horizontal = packed_wheel.mouse_wheel_horizontal;
			}
			// Text input
			else if (id == text_input_id)
			{
				auto & byte_it = reinterpret_cast<int8_t const *&>(it);
				int8_t const len = *byte_it++;
				std::copy(byte_it, byte_it + len, frame.text_input.data());
				byte_it += len;
			}
			// Mouse button
			else if (id >= key_count && id < key_count + mouse_button_count)
			{
				frame.buttons_down[id].flip();
			}
			// Key
			else
			{
				if (id & key_repeat_mask)
					frame.buttons_repeated[id & ~key_repeat_mask].flip();
				else
					frame.buttons_down[id].flip();
			}
		}

		return frame;
	}

	auto operator == (PreviousFrame const & a, PreviousFrame const & b) noexcept -> bool
	{
		return a.buttons_down == b.buttons_down
			&& a.buttons_repeated == b.buttons_repeated
			&& a.mouse_pos_x == b.mouse_pos_x
			&& a.mouse_pos_y == b.mouse_pos_y
			&& a.controllers_on == b.controllers_on
			&& a.controller_buttons == b.controller_buttons
			&& a.controller_axes == b.controller_axes;
	}

	auto operator != (PreviousFrame const & a, PreviousFrame const & b) noexcept -> bool
	{
		return !(a == b);
	}

	auto operator == (Frame const & a, Frame const & b) noexcept -> bool
	{
		return static_cast<PreviousFrame const &>(a) == static_cast<PreviousFrame const &>(b)
			&& a.mouse_wheel == b.mouse_wheel
			&& strncmp(a.text_input.data(), b.text_input.data(), a.text_input.size()) == 0;
	}

	auto operator != (Frame const & a, Frame const & b) noexcept -> bool
	{
		return !(a == b);
	}

	auto operator == (Input const & a, Input const & b) noexcept -> bool
	{
		return a.current == b.current && a.previous == b.previous;
	}

	auto operator != (Input const & a, Input const & b) noexcept -> bool
	{
		return !(a == b);
	}

} // namespace aeh::in
