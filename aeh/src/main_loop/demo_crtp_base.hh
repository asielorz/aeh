#pragma once

#ifdef AEH_WITH_SDL2

#include "main_loop.hh"
#include "tuple.hh"

struct SDL_Window;
union SDL_Event;

namespace aeh::main_loop
{
	namespace detail
	{
		template <typename ... Ts> struct type_list {};
		template <typename T, typename L> struct append_if_not_empty;
		template <typename T, typename ... Ts> struct append_if_not_empty<T, type_list<Ts...>> { using type = type_list<T, Ts...>; };
		template <typename ... Ts> struct append_if_not_empty<empty_t, type_list<Ts...>> { using type = type_list<Ts...>; };
		template <typename L, typename T> using append_if_not_empty_t = typename append_if_not_empty<L, T>::type;

		template <template <typename A, typename B> typename F, typename X, typename L>
		struct fold;
		template <template <typename A, typename B> typename F, typename X>
		struct fold<F, X, type_list<>> { using type = X; };
		template <template <typename A, typename B> typename F, typename X, typename First, typename ... List>
		struct fold<F, X, type_list<First, List...>> { using type = F<First, typename fold<F, X, type_list<List...>>::type>; };
		template <template <typename A, typename B> typename F, typename X, typename L>
		using fold_t = typename fold<F, X, L>::type;

		template <template <typename A> typename F, typename L>			struct map;
		template <template <typename A> typename F, typename ... List>	struct map<F, type_list<List...>> { using type = type_list<F<List>...>; };
		template <template <typename A> typename F, typename L>			using map_t = typename map<F, L>::type;

		template <template <typename ... Ts> typename F, typename L>		struct apply;
		template <template <typename ... Ts> typename F, typename ... Ts>	struct apply<F, type_list<Ts...>> { using type = F<Ts...>; };
		template <template <typename ... Ts> typename F, typename L>		using apply_t = typename apply<F, L>::type;

		template <typename ... Ts>
		struct InheritFromAll : public Ts... {};

		template <typename ... Ts>
		using keep_non_empty = fold_t<append_if_not_empty_t, type_list<>, type_list<Ts...>>;

		template <typename ... Ts>
		using InheritFromAllNonEmpty = apply_t<InheritFromAll, keep_non_empty<Ts...>>;

		template <typename Plugin>
		using plugin_update_input_extension = detail::void_to_empty<
			decltype(detail::call_update(
				std::declval<Plugin &>(),
				std::declval<aeh::main_loop::UpdateInput>(),
				std::declval<std::add_lvalue_reference_t<decltype(detail::call_start_frame(std::declval<Plugin &>()))>>()
			))
		>;
		template <typename Plugin>
		using plugin_render_input_extension = detail::void_to_empty<
			decltype(detail::call_pre_render(
				std::declval<Plugin &>(),
				std::declval<aeh::main_loop::RenderInput>()
			))
		>;

		template <typename ... Plugins>
		using UpdateInputForPlugins = InheritFromAllNonEmpty<aeh::main_loop::UpdateInput, plugin_update_input_extension<Plugins>...>;

		template <typename ... Plugins>
		using RenderInputForPlugins = InheritFromAllNonEmpty<aeh::main_loop::RenderInput, plugin_render_input_extension<Plugins>...>;

	} // namespace detail

	template <typename Impl, typename ... Plugins>
	struct CRTPBase
	{
		using UpdateInput = detail::UpdateInputForPlugins<Plugins...>;
		using RenderInput = detail::RenderInputForPlugins<Plugins...>;

		CRTPBase() noexcept((std::is_nothrow_default_constructible_v<Plugins> && ...)) = default;
		explicit CRTPBase(Plugins ... plugins_) noexcept((std::is_nothrow_move_constructible_v<Plugins> && ...)) requires(sizeof...(Plugins) > 0) : plugins(std::move(plugins_)...) {}

		void initialize(SDL_Window * window);
		auto start_frame();
		template <typename Locals> void update(aeh::main_loop::UpdateInput input, Locals && local);
		template <typename Locals> void render(aeh::main_loop::RenderInput input, Locals && locals) const;
		template <typename Locals> void process_event(SDL_Event const & event, Locals && locals);
		void shutdown();

		template <typename Plugin> Plugin & get_plugin() noexcept;
		template <typename Plugin> Plugin const & get_plugin() const noexcept;

	private:
		Impl & implementation() noexcept { return static_cast<Impl &>(*this); }
		Impl const & implementation() const noexcept { return static_cast<Impl const &>(*this); }

		std::tuple<Plugins...> plugins;
	};

} // namespace aeh::main_loop

#include "demo_crtp_base.inl"

#endif // AEH_WITH_SDL2
