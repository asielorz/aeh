#include "imgui_plugin.hh"
#include "main_loop.hh"
#include <imgui/imgui.h>

namespace aeh::main_loop
{

	void ImGuiPlugin::initialize(SDL_Window * window)
	{
		previous_context = ImGui::GetCurrentContext();
		context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);
		ImGui::SDL::Init(renderer, window);
		ImGui::StyleColorsDark();
	}

	void ImGuiPlugin::update(UpdateInput i)
	{
		ImGui::SDL::NewFrame(renderer, i.window);
	}

	void ImGuiPlugin::post_render() const
	{
		ImGui::Render();
		ImGui::SDL::RenderDrawData(renderer, ImGui::GetDrawData());
	}

	void ImGuiPlugin::shutdown()
	{
		ImGui::SDL::Shutdown(renderer);
		ImGui::DestroyContext(context);
		ImGui::SetCurrentContext(previous_context);
	}

	void ImGuiPlugin::process_event(SDL_Event const & event)
	{
		ImGui::SDL::ProcessEvent(renderer, &event);
	}

} // namespace aeh::main_loop
