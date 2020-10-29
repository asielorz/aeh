#pragma once

#ifdef AEH_WITH_SDL2

#include "main_loop.hh"
#include "tuple.hh"

struct SDL_Window;
union SDL_Event;

namespace aeh::main_loop
{

	template <typename Impl, typename ... Plugins>
	struct CRTPBase
	{
		template <typename Plugin>
		using plugin_update_input_extension = detail::void_to_empty<
			decltype(detail::call_update(
				std::declval<Plugin &>(), 
				std::declval<aeh::main_loop::UpdateInput>(), 
				std::declval<std::add_lvalue_reference_t<decltype(detail::call_start_frame(std::declval<Plugin &>()))>>()
			))
		>;

		struct UpdateInput : aeh::main_loop::UpdateInput, plugin_update_input_extension<Plugins>... {};

		void initialize(SDL_Window * window);
		auto start_frame();
		template <typename Locals> void update(aeh::main_loop::UpdateInput input, Locals && local);
		template <typename Locals> void render(Locals && locals) const;
		template <typename Locals> void process_event(SDL_Event const & event, Locals && locals);
		void shutdown();


	private:
		Impl & implementation() noexcept { return static_cast<Impl &>(*this); }
		Impl const & implementation() const noexcept { return static_cast<Impl const &>(*this); }

		std::tuple<Plugins...> plugins;
	};

} // namespace aeh::main_loop

#include "demo_crtp_base.inl"

#endif // AEH_WITH_SDL2
