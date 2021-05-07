#include "viewport.hh"
#include "gl/gl_core_4_5.hh"

#if AEH_WITH_GLM
	#include <glm/glm.hpp>
#endif // AEH_WITH_GLM

namespace aeh
{

	auto Viewport::current() -> Viewport
	{
		int values[4];
		gl::GetIntegerv(gl::VIEWPORT, values);
		return {values[0], values[1], values[2], values[3]};
	}

	void set_as_current_viewport(Viewport viewport)
	{
		gl::Viewport(viewport.pos_x, viewport.pos_y, viewport.size_x, viewport.size_y);
	}

	auto biggest_viewport_with_desired_aspect_ratio(float max_width, float max_height, float desired_aspect_ratio) noexcept -> ViewportFloat
	{
		float const aspect_ratio = static_cast<float>(max_width) / max_height;
		if (aspect_ratio > desired_aspect_ratio)
		{
			// Keep height, reduce width
			float const width = max_height * desired_aspect_ratio;
			return {
					(max_width - width) / 2, 0,
					width, max_height
			};
		}
		else
		{
			// Keep width, reduce height to fit desired aspect ratio
			float const height = max_width / desired_aspect_ratio;
			return {
					0, (max_height - height) / 2,
					max_width, height
			};
		}
	}

#if AEH_WITH_GLM
	auto viewport_in_ndc(ViewportFloat viewport_ndc, float window_width, float window_height) noexcept -> Viewport
	{
		return viewport_in_ndc({viewport_ndc.pos_x, viewport_ndc.pos_y}, {viewport_ndc.size_x, viewport_ndc.size_y}, window_width, window_height);
	}

	auto viewport_in_ndc(glm::vec2 pos, glm::vec2 size, float window_width, float window_height) noexcept -> Viewport
	{
		glm::vec2 const size_screen = size * window_height * 0.5f;
		glm::vec2 const pos_screen = ndc_aspect_ratio_to_gl_screen(pos - (size * 0.5f), window_width, window_height);
		return {
			static_cast<int>(pos_screen.x), static_cast<int>(pos_screen.y),
			static_cast<int>(size_screen.x), static_cast<int>(size_screen.y)
		};
	}

	glm::vec2 ndc_aspect_ratio_to_gl_screen(glm::vec2 pos_ndc, float window_width, float window_height) noexcept
	{
		return {
			pos_ndc.x * window_height / 2 + window_width / 2,
			pos_ndc.y * window_height / 2 + window_height / 2
		};
	}
#endif // AEH_WITH_GLM

	PushViewport::PushViewport(Viewport viewport)
		: previous_viewport(Viewport::current())
	{
		set_as_current_viewport(viewport);
	}

	PushViewport::~PushViewport()
	{
		set_as_current_viewport(previous_viewport);
	}


} // namespace aeh
