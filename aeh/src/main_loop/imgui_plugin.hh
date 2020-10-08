#pragma once

#define IMGUI_API
#include "imgui/imgui_impl_sdl_gl3.h"
struct ImGuiContext;
struct SDL_Window;
union SDL_Event;

namespace aeh::main_loop
{
	struct UpdateInput;

	struct ImGuiPlugin
	{
		void initialize(SDL_Window * window);
		void update(UpdateInput i);
		void post_render() const;
		void shutdown();
		void process_event(SDL_Event const & event);

	private:
		ImGui::SDL::Renderer renderer;
		ImGuiContext * context;
		ImGuiContext * previous_context;
	};

} // namespace aeh::main_loop
