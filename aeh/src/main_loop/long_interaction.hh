#pragma once

#include "main_loop.hh"
#include "demo_crtp_base.hh"
#include "input_plugin.hh"

namespace aeh::main_loop
{

	using LongInteractionUpdateInput = detail::UpdateInputForPlugins<InputPlugin>;
	using LongInteractionRenderInput = detail::RenderInputForPlugins<InputPlugin>;

	template <typename Upstream, typename Update>
	struct LongInteractionLoop : CRTPBase<LongInteractionLoop<Upstream, Update>, InputPlugin>
	{
		LongInteractionLoop(Upstream & upstream_loop_, Update update_function_) noexcept 
			: upstream_loop(std::addressof(upstream_loop_))
			, update_function(std::move(update_function_))
		{}

		auto update_impl(LongInteractionUpdateInput loop) -> void;
		auto render_impl(LongInteractionRenderInput loop) const -> void;

	private:
		Upstream * upstream_loop;
		Update update_function;
	};

	template <typename Upstream, typename InteractionUpdate>
	auto long_interaction(Upstream & upstream, SDL_Window * window, InteractionUpdate && interaction_update) -> int
	{
		return aeh::main_loop::run(LongInteractionLoop(upstream, std::forward<InteractionUpdate>(interaction_update)), window);
	}

} // namespace aeh::main_loop

#include "long_interaction.inl"
