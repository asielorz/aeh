#ifdef AEH_HAS_SDL

#include "input_SDL.hh"
#include "input.hh"
#include "debug/unreachable.hh"
#include <SDL2/SDL.h>

namespace aeh::in
{

	auto SDL_scancode_to_key(int scancode) noexcept -> Key
 	{
		switch (scancode)
		{
			case SDL_SCANCODE_A: return Key::a;
			case SDL_SCANCODE_B: return Key::b;
			case SDL_SCANCODE_C: return Key::c;
			case SDL_SCANCODE_D: return Key::d;
			case SDL_SCANCODE_E: return Key::e;
			case SDL_SCANCODE_F: return Key::f;
			case SDL_SCANCODE_G: return Key::g;
			case SDL_SCANCODE_H: return Key::h;
			case SDL_SCANCODE_I: return Key::i;
			case SDL_SCANCODE_J: return Key::j;
			case SDL_SCANCODE_K: return Key::k;
			case SDL_SCANCODE_L: return Key::l;
			case SDL_SCANCODE_M: return Key::m;
			case SDL_SCANCODE_N: return Key::n;
			case SDL_SCANCODE_O: return Key::o;
			case SDL_SCANCODE_P: return Key::p;
			case SDL_SCANCODE_Q: return Key::q;
			case SDL_SCANCODE_R: return Key::r;
			case SDL_SCANCODE_S: return Key::s;
			case SDL_SCANCODE_T: return Key::t;
			case SDL_SCANCODE_U: return Key::u;
			case SDL_SCANCODE_V: return Key::v;
			case SDL_SCANCODE_W: return Key::w;
			case SDL_SCANCODE_X: return Key::x;
			case SDL_SCANCODE_Y: return Key::y;
			case SDL_SCANCODE_Z: return Key::z;

			case SDL_SCANCODE_1: return Key::top1;
			case SDL_SCANCODE_2: return Key::top2;
			case SDL_SCANCODE_3: return Key::top3;
			case SDL_SCANCODE_4: return Key::top4;
			case SDL_SCANCODE_5: return Key::top5;
			case SDL_SCANCODE_6: return Key::top6;
			case SDL_SCANCODE_7: return Key::top7;
			case SDL_SCANCODE_8: return Key::top8;
			case SDL_SCANCODE_9: return Key::top9;
			case SDL_SCANCODE_0: return Key::top0;

			case SDL_SCANCODE_RETURN: return Key::enter;
			case SDL_SCANCODE_ESCAPE: return Key::escape;
			case SDL_SCANCODE_BACKSPACE: return Key::backspace;
			case SDL_SCANCODE_TAB: return Key::tab;
			case SDL_SCANCODE_SPACE: return Key::space;

			case SDL_SCANCODE_MINUS: return Key::minus;
			case SDL_SCANCODE_EQUALS: return Key::plus;
			case SDL_SCANCODE_LEFTBRACKET: return Key::open_square_curly_brace;
			case SDL_SCANCODE_RIGHTBRACKET: return Key::close_square_curly_brace;
			case SDL_SCANCODE_BACKSLASH: return Key::backslash_vertical_bar;
			case SDL_SCANCODE_SEMICOLON: return Key::colon_semi_colon;
			case SDL_SCANCODE_APOSTROPHE: return Key::single_double_quote;
			case SDL_SCANCODE_GRAVE: return Key::grave_accent_tilde;
			case SDL_SCANCODE_COMMA: return Key::comma;
			case SDL_SCANCODE_PERIOD: return Key::period;
			case SDL_SCANCODE_SLASH: return Key::slash_question_mark;
			case SDL_SCANCODE_CAPSLOCK: return Key::caps_lock;

			case SDL_SCANCODE_F1: return Key::f1;
			case SDL_SCANCODE_F2: return Key::f2;
			case SDL_SCANCODE_F3: return Key::f3;
			case SDL_SCANCODE_F4: return Key::f4;
			case SDL_SCANCODE_F5: return Key::f5;
			case SDL_SCANCODE_F6: return Key::f6;
			case SDL_SCANCODE_F7: return Key::f7;
			case SDL_SCANCODE_F8: return Key::f8;
			case SDL_SCANCODE_F9: return Key::f9;
			case SDL_SCANCODE_F10: return Key::f10;
			case SDL_SCANCODE_F11: return Key::f11;
			case SDL_SCANCODE_F12: return Key::f12;
			case SDL_SCANCODE_F13: return Key::f13;
			case SDL_SCANCODE_F14: return Key::f14;
			case SDL_SCANCODE_F15: return Key::f15;
			case SDL_SCANCODE_F16: return Key::f16;
			case SDL_SCANCODE_F17: return Key::f17;
			case SDL_SCANCODE_F18: return Key::f18;
			case SDL_SCANCODE_F19: return Key::f19;
			case SDL_SCANCODE_F20: return Key::f20;
			case SDL_SCANCODE_F21: return Key::f21;
			case SDL_SCANCODE_F22: return Key::f22;
			case SDL_SCANCODE_F23: return Key::f23;
			case SDL_SCANCODE_F24: return Key::f24;

			case SDL_SCANCODE_PRINTSCREEN: return Key::print_screen;
			case SDL_SCANCODE_SCROLLLOCK: return Key::scroll_lock;
			case SDL_SCANCODE_PAUSE: return Key::pause;
			case SDL_SCANCODE_INSERT: return Key::insert;
			case SDL_SCANCODE_HOME: return Key::home;
			case SDL_SCANCODE_PAGEUP: return Key::page_up;
			case SDL_SCANCODE_DELETE: return Key::del;
			case SDL_SCANCODE_END: return Key::end;
			case SDL_SCANCODE_PAGEDOWN: return Key::page_down;
			case SDL_SCANCODE_RIGHT: return Key::arrow_right;
			case SDL_SCANCODE_LEFT: return Key::arrow_left;
			case SDL_SCANCODE_UP: return Key::arrow_up;
			case SDL_SCANCODE_DOWN: return Key::arrow_down;

			case SDL_SCANCODE_KP_DIVIDE: return Key::divide;
			case SDL_SCANCODE_KP_MULTIPLY: return Key::multiply;
			case SDL_SCANCODE_KP_MINUS: return Key::subtract;
			case SDL_SCANCODE_KP_PLUS: return Key::add;
			//case SDL_SCANCODE_KP_ENTER: return Key::;
			//case SDL_SCANCODE_KP_PERIOD: return Key::;
			case SDL_SCANCODE_KP_1: return Key::numpad1;
			case SDL_SCANCODE_KP_2: return Key::numpad2;
			case SDL_SCANCODE_KP_3: return Key::numpad3;
			case SDL_SCANCODE_KP_4: return Key::numpad4;
			case SDL_SCANCODE_KP_5: return Key::numpad5;
			case SDL_SCANCODE_KP_6: return Key::numpad6;
			case SDL_SCANCODE_KP_7: return Key::numpad7;
			case SDL_SCANCODE_KP_8: return Key::numpad8;
			case SDL_SCANCODE_KP_9: return Key::numpad9;
			case SDL_SCANCODE_KP_0: return Key::numpad0;

			case SDL_SCANCODE_EXECUTE: return Key::execute;
			case SDL_SCANCODE_HELP: return Key::help;
			case SDL_SCANCODE_MENU: return Key::alt;
			case SDL_SCANCODE_SELECT: return Key::select;
			case SDL_SCANCODE_MUTE: return Key::volume_mute;
			case SDL_SCANCODE_VOLUMEUP: return Key::volume_up;
			case SDL_SCANCODE_VOLUMEDOWN: return Key::volume_down;

			case SDL_SCANCODE_LCTRL: return Key::left_control;
			case SDL_SCANCODE_LSHIFT: return Key::left_shift;
			case SDL_SCANCODE_LALT: return Key::left_alt;
			case SDL_SCANCODE_LGUI: return Key::left_windows;
			case SDL_SCANCODE_RCTRL: return Key::right_control;
			case SDL_SCANCODE_RSHIFT: return Key::right_shift;
			case SDL_SCANCODE_RALT: return Key::right_alt;
			case SDL_SCANCODE_RGUI: return Key::right_windows;

			case SDL_SCANCODE_LANG1: return Key::hangul;
			case SDL_SCANCODE_LANG2: return Key::hanja;

			case SDL_SCANCODE_CLEAR: return Key::clear;
			case SDL_SCANCODE_SEPARATOR: return Key::separator;
			case SDL_SCANCODE_APP1: return Key::launch_app1;
			case SDL_SCANCODE_APP2: return Key::launch_app2;
		}

		debug::declare_unreachable();
	}

	auto SDL_controller_button_to_enum(int button) -> ControllerButton
	{
		switch (button)
		{
			case SDL_CONTROLLER_BUTTON_A: return ControllerButton::a;
			case SDL_CONTROLLER_BUTTON_B: return ControllerButton::b;
			case SDL_CONTROLLER_BUTTON_X: return ControllerButton::x;
			case SDL_CONTROLLER_BUTTON_Y: return ControllerButton::y;
			case SDL_CONTROLLER_BUTTON_BACK: return ControllerButton::select;
			case SDL_CONTROLLER_BUTTON_START: return ControllerButton::start;
			case SDL_CONTROLLER_BUTTON_LEFTSTICK: return ControllerButton::left_joystick;
			case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return ControllerButton::right_joystick;
			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return ControllerButton::LB;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return ControllerButton::RB;
			case SDL_CONTROLLER_BUTTON_DPAD_UP: return ControllerButton::up;
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return ControllerButton::down;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return ControllerButton::left;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return ControllerButton::right;
		}

		debug::declare_unreachable();
	}

	auto SDL_controller_axis_to_enum(int button) -> ControllerAxis
	{
		switch (button)
		{
			case SDL_CONTROLLER_AXIS_LEFTX: return ControllerAxis::left_joystick_x;
			case SDL_CONTROLLER_AXIS_LEFTY: return ControllerAxis::left_joystick_y;
			case SDL_CONTROLLER_AXIS_RIGHTX: return ControllerAxis::right_joystick_x;
			case SDL_CONTROLLER_AXIS_RIGHTY: return ControllerAxis::right_joystick_y;
			case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return ControllerAxis::left_trigger;
			case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return ControllerAxis::right_trigger;
		}

		debug::declare_unreachable();
	}

	auto prepare_frame_to_process_SDL_events(PreviousFrame const & prev) noexcept -> Frame
	{
		Frame frame;
		frame.buttons_down = prev.buttons_down;
		frame.mouse_pos_x = prev.mouse_pos_x;
		frame.mouse_pos_y = prev.mouse_pos_y;
		frame.window_height = prev.window_height;
		frame.window_width = prev.window_width;
		frame.controllers_on = prev.controllers_on;
		frame.controller_buttons = prev.controller_buttons;
		frame.controller_axes = prev.controller_axes;

		return frame;
	}

	auto process_SDL_event(Frame & frame, SDL_Event const & event) noexcept -> void
	{
		switch (event.type)
		{
			case SDL_MOUSEWHEEL:
			{
				if (event.wheel.y > 0) frame.mouse_wheel += 1;
				if (event.wheel.y < 0) frame.mouse_wheel -= 1;
				if (event.wheel.x > 0) frame.mouse_wheel_horizontal += 1;
				if (event.wheel.x < 0) frame.mouse_wheel_horizontal -= 1;
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if (event.button.button == SDL_BUTTON_LEFT) frame.buttons_down[index_of(MouseButton::left)] = true;
				if (event.button.button == SDL_BUTTON_RIGHT) frame.buttons_down[index_of(MouseButton::right)] = true;
				if (event.button.button == SDL_BUTTON_MIDDLE) frame.buttons_down[index_of(MouseButton::middle)] = true;
				if (event.button.button == SDL_BUTTON_X1) frame.buttons_down[index_of(MouseButton::x1)] = true;
				if (event.button.button == SDL_BUTTON_X2) frame.buttons_down[index_of(MouseButton::x2)] = true;
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				if (event.button.button == SDL_BUTTON_LEFT) frame.buttons_down[index_of(MouseButton::left)] = false;
				if (event.button.button == SDL_BUTTON_RIGHT) frame.buttons_down[index_of(MouseButton::right)] = false;
				if (event.button.button == SDL_BUTTON_MIDDLE) frame.buttons_down[index_of(MouseButton::middle)] = false;
				if (event.button.button == SDL_BUTTON_X1) frame.buttons_down[index_of(MouseButton::x1)] = false;
				if (event.button.button == SDL_BUTTON_X2) frame.buttons_down[index_of(MouseButton::x2)] = false;
				break;
			}
			case SDL_TEXTINPUT:
			{
				strcpy_s(frame.text_input.data(), frame.text_input.size(), event.text.text);
				break;
			}
			case SDL_KEYDOWN:
			{
				auto const index = index_of(SDL_scancode_to_key(event.key.keysym.scancode));
				if (frame.buttons_down[index])
					frame.buttons_repeated[index] = true;
				else
					frame.buttons_down[index] = true;

				break;
			}
			case SDL_KEYUP:
			{
				auto const index = index_of(SDL_scancode_to_key(event.key.keysym.scancode));
				frame.buttons_down[index] = false;

				break;
			}
			case SDL_MOUSEMOTION:
			{
				frame.mouse_pos_x = static_cast<int16_t>(event.motion.x);
				frame.mouse_pos_y = static_cast<int16_t>(event.motion.y);

				break;
			}
			case SDL_CONTROLLERDEVICEADDED:
			{
				int const index = event.cdevice.which;
				frame.controllers_on[index] = true;
				SDL_GameControllerOpen(index);
				break;
			}
			case SDL_CONTROLLERDEVICEREMOVED:
			{
				int const index = event.cdevice.which;
				frame.controllers_on[index] = false;
			}
			case SDL_CONTROLLERBUTTONDOWN:
			{
				int const index = controller_button_index(static_cast<Controller>(event.cbutton.which) | SDL_controller_button_to_enum(event.cbutton.button));
				frame.controller_buttons[index] = true;
				break;
			}
			case SDL_CONTROLLERBUTTONUP:
			{
				int const index = controller_button_index(controller_enum_from_index(event.cbutton.which) | SDL_controller_button_to_enum(event.cbutton.button));
				frame.controller_buttons[index] = false;
				break;
			}
			
			case SDL_CONTROLLERAXISMOTION:
			{
				ControllerAxis const axis = SDL_controller_axis_to_enum(event.caxis.axis);
				Controller const controller = controller_enum_from_index(event.caxis.which);
				auto const [controller_index, axis_index] = controller_axis_index(controller | axis);
				float const value = event.caxis.value / static_cast<float>(std::numeric_limits<short>::max());

				// For some reason SDL gives the y axes negated.
				if (axis == ControllerAxis::left_joystick_y || axis == ControllerAxis::right_joystick_y)
					frame.controller_axes[controller_index][axis_index] = -value;
				else
					frame.controller_axes[controller_index][axis_index] = value;

				break;
			}

			case SDL_WINDOWEVENT:
			{
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					frame.window_width = static_cast<int16_t>(event.window.data1);
					frame.window_height = static_cast<int16_t>(event.window.data2);
				} 
				break;
			}
		}
	}
	
} // namespace aeh::in

#endif // AEH_HAS_SDL
