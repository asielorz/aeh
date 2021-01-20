#pragma once

#if defined AEH_WITH_SDL2 && defined AEH_WITH_IMGUI

#include "imgui_sdl_binding.hh"

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

	protected:
		ImGui_SDL::Renderer renderer;
		ImGuiContext * context;
		ImGuiContext * previous_context;
	};

} // namespace aeh::main_loop

#endif // AEH_WITH_SDL2 && AEH_WITH_IMGUI
