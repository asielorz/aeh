#pragma once

#if defined AEH_WITH_SDL2 && defined AEH_WITH_IMGUI

#include "main_loop/demo_crtp_base.hh"
#include "main_loop/imgui_plugin.hh"
#include "main_loop/input_plugin.hh"

#include <imgui.h>

namespace aeh
{
	using ImGuiDialogUpdateInput = main_loop::detail::UpdateInputForPlugins<main_loop::InputPlugin, main_loop::ImGuiPlugin>;

	namespace detail
	{
		template <std::invocable<ImGuiDialogUpdateInput> F>
		struct ImDialogLoop : main_loop::CRTPBase<ImDialogLoop<F>, main_loop::InputPlugin, main_loop::ImGuiPlugin>
		{
			using Base = main_loop::CRTPBase<ImDialogLoop<F>, main_loop::InputPlugin, main_loop::ImGuiPlugin>;
			using UpdateInput = typename Base::UpdateInput;

			explicit ImDialogLoop(F f) noexcept : update_function(std::move(f)) {}

			void update_impl(UpdateInput loop) 
			{
				ImGui::SetNextWindowPos({0, 0});
				ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
				if (ImGui::Begin("The name of this window doesn't matter", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
				{
					update_function(loop); 
					ImGui::End();
				}
			}

		private:
			F update_function;
		};
	}

	template <std::invocable<ImGuiDialogUpdateInput> F>
	auto imgui_dialog(main_loop::NewWindowOptions new_window_options, F && f) -> int
	{
		return main_loop::run(detail::ImDialogLoop(std::forward<F>(f)), new_window_options);
	}

} // namespace aeh

#endif // AEH_WITH_SDL2 && AEH_WITH_IMGUI
