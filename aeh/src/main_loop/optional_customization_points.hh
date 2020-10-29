#pragma once

#ifdef AEH_WITH_SDL2

#include "priority.hh"
#include <utility> // forward

struct SDL_Window;
union SDL_Event;

#define AEH_MAIN_LOOP_DECLARE_CALL_IF_EXISTS(name)																		\
	template <typename T, typename ... Args>																			\
	auto z_call_##name(priority_tag<1>, T && t, Args && ... args) 														\
		-> decltype(t.name(std::forward<Args>(args)...))																\
	{																													\
		return t.name(std::forward<Args>(args)...);																		\
	}																													\
																														\
	template <typename T, typename ... Args>																			\
	auto z_call_##name(priority_tag<0>, T &&, Args && ...)																\
	{}																													\
																														\
	template <typename T, typename ... Args>																			\
	auto call_##name(T && t, Args && ... args) -> void																	\
	{																													\
		z_call_##name(priority_tag_v<1>, std::forward<T>(t), std::forward<Args>(args)...);								\
	}																													\


namespace aeh::main_loop::detail
{

	struct empty_t {};

	template <typename T>
	using void_to_empty = std::conditional_t<std::is_same_v<T, void>, empty_t, T>;

	AEH_MAIN_LOOP_DECLARE_CALL_IF_EXISTS(initialize)
	AEH_MAIN_LOOP_DECLARE_CALL_IF_EXISTS(initialize_impl)
	AEH_MAIN_LOOP_DECLARE_CALL_IF_EXISTS(shutdown)
	AEH_MAIN_LOOP_DECLARE_CALL_IF_EXISTS(shutdown_impl)
	AEH_MAIN_LOOP_DECLARE_CALL_IF_EXISTS(process_event)
	AEH_MAIN_LOOP_DECLARE_CALL_IF_EXISTS(process_event_impl)
	AEH_MAIN_LOOP_DECLARE_CALL_IF_EXISTS(pre_render)
	AEH_MAIN_LOOP_DECLARE_CALL_IF_EXISTS(post_render)

	template <typename T, typename = decltype(std::declval<T>().start_frame())>
	auto z_call_start_frame(T & t, priority_tag<1>)
	{
		if constexpr (std::is_same_v<decltype(t.start_frame()), void>)
		{
			t.start_frame();
			return empty_t();
		}
		else
		{
			return t.start_frame();
		}
	}

	template <typename T>
	auto z_call_start_frame(T &, priority_tag<0>)
	{
		return empty_t();
	}

	template <typename T>
	auto call_start_frame(T & t)
	{
		return z_call_start_frame(t, priority_tag_v<1>);
	}

	template <typename T, typename = decltype(std::declval<T>().start_frame())>
	auto z_call_start_frame_impl(T & t, priority_tag<1>)
	{
		if constexpr (std::is_same_v<decltype(t.start_frame_impl()), void>)
		{
			t.start_frame_impl();
			return empty_t();
		}
		else
		{
			return t.start_frame_impl();
		}
	}

	template <typename T>
	auto z_call_start_frame_impl(T &, priority_tag<0>)
	{
		return empty_t();
	}

	template <typename T>
	auto call_start_frame_impl(T & t)
	{
		return z_call_start_frame(t, priority_tag_v<1>);
	}

	template <typename T, typename UpdateInput, typename Locals>
	auto call_update(T & t, UpdateInput && input, Locals & locals)
	{
		if constexpr (std::is_same_v<void, decltype(t.update(input, locals))>)
		{
			t.update(input, locals);
			return empty_t();
		}
		else
		{
			return t.update(input, locals);
		}
	}

	template <typename T, typename UpdateInput>
	auto call_update(T & t, UpdateInput && input, empty_t)
	{
		if constexpr (std::is_same_v<void, decltype(t.update(input))>)
		{
			t.update(input);
			return empty_t();
		}
		else
		{
			return t.update(input);
		}
	}

	template <typename T, typename UpdateInput, typename Locals>
	auto call_update_impl(T & t, UpdateInput && input, Locals & locals)
	{
		return t.update_impl(input, locals);
	}

	template <typename T, typename UpdateInput>
	auto call_update_impl(T & t, UpdateInput & input, empty_t)
	{
		return t.update_impl(input);
	}

	template <typename Demo, typename Locals>
	void render_demo(Demo const & demo, Locals & locals)
	{
		if constexpr (std::is_same_v<Locals, empty_t>)
		{
			demo.render();
			static_cast<void>(locals);
		}
		else
		{
			demo.render(locals);
		}
	}

	template <typename Demo, typename Locals>
	auto z_call_render_impl(priority_tag<1>, Demo const & demo, Locals & locals) -> decltype(demo.render_impl(locals))
	{
		return demo.render_impl(locals);
	}
	template <typename Demo, typename Locals>
	auto z_call_render_impl(priority_tag<1>, Demo const & demo, empty_t) -> decltype(demo.render_impl())
	{
		return demo.render_impl();
	}
	template <typename Demo, typename Locals>
	void z_call_render_impl(priority_tag<0>, Demo const &, Locals &)
	{}

	template <typename Demo, typename Locals>
	void call_render_impl(Demo const & demo, Locals & locals)
	{
		z_call_render_impl(priority_tag_v<1>, demo, locals);
	}

	//****************************************************************

	template <typename Demo, typename Locals>
	auto z_call_process_event(priority_tag<1>, Demo const & demo, SDL_Event const & event, Locals && locals) -> decltype(demo.process_event(event, locals))
	{
		return demo.process_event(event, locals);
	}
	template <typename Demo, typename Locals>
	auto z_call_process_event(priority_tag<1>, Demo const & demo, SDL_Event const & event, empty_t) -> decltype(demo.process_event(event))
	{
		return demo.process_event(event);
	}
	template <typename Demo, typename Locals>
	void z_call_process_event(priority_tag<0>, Demo const &, SDL_Event const &, Locals &&)
	{}

	template <typename Demo, typename Locals>
	void call_process_event(Demo const & demo, SDL_Event const & event, Locals && locals)
	{
		z_call_process_event(priority_tag_v<1>, demo, event, locals);
	}

	//****************************************************************

	template <typename Demo, typename Locals>
	auto z_call_process_event_impl(priority_tag<1>, Demo const & demo, SDL_Event const & event, Locals && locals) -> decltype(demo.process_event_impl(event, locals))
	{
		return demo.process_event_impl(event, locals);
	}
	template <typename Demo, typename Locals>
	auto z_call_process_event_impl(priority_tag<1>, Demo const & demo, SDL_Event const & event, empty_t) -> decltype(demo.process_event_impl(event))
	{
		return demo.process_event_impl(event);
	}
	template <typename Demo, typename Locals>
	void z_call_process_event_impl(priority_tag<0>, Demo const &, SDL_Event const &, Locals &&)
	{}

	template <typename Demo, typename Locals>
	void call_process_event_impl(Demo const & demo, SDL_Event const & event, Locals && locals)
	{
		z_call_process_event_impl(priority_tag_v<1>, demo, event, locals);
	}

} // namespace aeh::main_loop::detail

#undef AEH_MAIN_LOOP_DECLARE_CALL_IF_EXISTS

#endif // AEH_WITH_SDL2
