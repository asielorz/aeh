#if defined AEH_WITH_SDL2 && defined AEH_WITH_IMGUI

#include "imgui_plugin.hh"
#include "main_loop.hh"
#include <imgui.h>

namespace aeh::main_loop
{

	void ImGuiPlugin::initialize(SDL_Window * window)
	{
		previous_context = ImGui::GetCurrentContext();
		context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);
		ImGui_SDL::Init(renderer, window);
		ImGui::StyleColorsDark();
	}

	void ImGuiPlugin::update(UpdateInput i)
	{
		ImGui_SDL::NewFrame(renderer, i.window);
	}

	void ImGuiPlugin::post_render() const
	{
		ImGui::Render();
		ImGui_SDL::RenderDrawData(renderer, ImGui::GetDrawData());
	}

	void ImGuiPlugin::shutdown()
	{
		ImGui_SDL::Shutdown(renderer);
		ImGui::DestroyContext(context);
		ImGui::SetCurrentContext(previous_context);
	}

	void ImGuiPlugin::process_event(SDL_Event const & event)
	{
		ImGui_SDL::ProcessEvent(renderer, &event);
	}

} // namespace aeh::main_loop

#endif // AEH_WITH_SDL2 && AEH_WITH_IMGUI
