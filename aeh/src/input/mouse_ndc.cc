#pragma once

#ifdef AEH_WITH_GLM

#include "input.hh"
#include <glm/vec2.hpp>

namespace aeh::in
{
	struct Input;

	auto screen_to_ndc_aspect_ratio(in::Input const & input, uint16_t x, uint16_t y) noexcept -> glm::vec2
	{
		auto const window = glm::vec2(window_width(input), window_height(input));
		float const y_ = -(y - (window.y / 2)) / (window.y / 2);
		float const aspect_ratio = window.x / window.y;
		float const x_ = (x - (window.x / 2)) / (window.x / (2 * aspect_ratio));

		return {x_, y_};
	}

	auto mouse_pos_ndc_aspect_ratio(in::Input const & input) noexcept -> glm::vec2
	{
		return screen_to_ndc_aspect_ratio(input, mouse_pos_x(input), mouse_pos_y(input));
	}

	auto mouse_move_ndc_aspect_ratio(in::Input const & input) noexcept -> glm::vec2
	{
		auto const x = mouse_move_x(input);
		auto const y = mouse_move_y(input);

		auto const window = glm::vec2(window_width(input), window_height(input));
		float const y_ = -y / (window.y / 2);
		float const aspect_ratio = window.x / window.y;
		float const x_ = x / (window.x / (2 * aspect_ratio));

		return {x_, y_};
	}

} // namespace aeh::in

#endif // AEH_WITH_GLM
