#pragma once

#if AEH_WITH_GLM
	#include <glm/glm.hpp>
#endif // AEH_WITH_GLM

namespace aeh
{

	struct Viewport
	{
		int pos_x, pos_y, size_x, size_y;

		auto aspect_ratio() const noexcept { return static_cast<float>(size_x) / size_y; }

		[[nodiscard]] auto offset_by(int x, int y) const noexcept -> Viewport { return { pos_x + x, pos_y + y, size_x, size_y }; }
		[[nodiscard]] static auto current() -> Viewport;
	};

	struct ViewportFloat
	{
		float pos_x, pos_y, size_x, size_y;

		[[nodiscard]] auto clamp() const noexcept -> Viewport { return { static_cast<int>(pos_x), static_cast<int>(pos_y), static_cast<int>(size_x), static_cast<int>(size_y) }; }
		[[nodiscard]] auto offset_by(float x, float y) const noexcept -> ViewportFloat { return { pos_x + x, pos_y + y, size_x, size_y }; }
	};

	void set_as_current_viewport(Viewport viewport);
	[[nodiscard]] auto biggest_viewport_with_desired_aspect_ratio(float max_width, float max_height, float desired_aspect_ratio) noexcept -> ViewportFloat;

#if AEH_WITH_GLM
	[[nodiscard]] auto viewport_in_ndc(ViewportFloat viewport_ndc, float window_width, float window_height) noexcept -> Viewport;
	[[nodiscard]] auto viewport_in_ndc(glm::vec2 pos, glm::vec2 size, float window_width, float window_height) noexcept -> Viewport;
	[[nodiscard]] glm::vec2 ndc_aspect_ratio_to_gl_screen(glm::vec2 pos_ndc, float window_width, float window_height) noexcept;
#endif // AEH_WITH_GLM

	struct PushViewport
	{
		[[nodiscard]] explicit PushViewport(Viewport viewport);
		~PushViewport();
		PushViewport(PushViewport const &) = delete;
		PushViewport & operator = (PushViewport const &) = delete;

	private:
		Viewport previous_viewport;
	};

} // namespace aeh
