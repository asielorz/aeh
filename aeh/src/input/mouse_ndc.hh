#pragma once

#include <glm/fwd.hpp>

namespace aeh::in
{
	struct Input;

	[[nodiscard]] auto screen_to_ndc_aspect_ratio(in::Input const & input, uint16_t x, uint16_t y) noexcept -> glm::vec2;
	[[nodiscard]] auto mouse_pos_ndc_aspect_ratio(in::Input const & input) noexcept -> glm::vec2;
	[[nodiscard]] auto mouse_move_ndc_aspect_ratio(in::Input const & input) noexcept -> glm::vec2;

} // namespace aeh::in
