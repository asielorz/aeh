#pragma once

#include "function_ptr.hh"

/*
*	Non owning polymorphic callable object wrapper.
*	Trivially copyable and destructible.
* 
*	template <typename Ret, typename ... Args>
*	struct function_ref<Ret(Args...) noexcept> // Noexcept is optional. Will make operator () noexcept
*	{
*		constexpr function_ref() noexcept = default;
*		constexpr function_ref(std::nullptr_t) noexcept;
*		
*		template <typename Callable>
*		constexpr function_ref(Callable & c) noexcept;
*
*		template <auto F>
*		static constexpr function_ref to_constant;
* 
*		[[nodiscard]] constexpr auto has_value() const noexcept -> bool;
*		constexpr explicit operator bool() const noexcept;
*
*		[[nodiscard]] constexpr auto operator == (function_ref_base other) const noexcept -> bool;
*		[[nodiscard]] constexpr auto operator != (function_ref_base other) const noexcept -> bool;
* 
*		constexpr Ret operator () (Args ... args) const; //noexcept if function_ref is given a noexcept function type
*	};
*/

namespace aeh
{

	template <typename T> 
	struct function_ref;

	namespace detail
	{
		template <typename Caller>
		struct function_ref_base
		{
			constexpr function_ref_base() noexcept = default;
			constexpr function_ref_base(void * context_, Caller caller_) noexcept : context(context_), caller(caller_) {}

			[[nodiscard]] constexpr auto has_value() const noexcept -> bool;
			constexpr explicit operator bool() const noexcept;

			[[nodiscard]] constexpr auto operator == (function_ref_base other) const noexcept -> bool;
			[[nodiscard]] constexpr auto operator != (function_ref_base other) const noexcept -> bool;

		protected:
			void * context = nullptr;
			Caller caller = nullptr;
		};
	} // namespace detail

	template <typename Ret, typename ... Args>
	struct function_ref<Ret(Args...)> final : public detail::function_ref_base<function_ptr<Ret(void *, Args...)>>
	{
		constexpr function_ref() noexcept = default;
		constexpr function_ref(std::nullptr_t) noexcept {};

		template <typename Callable, typename = std::enable_if_t<std::is_invocable_r_v<Ret, Callable &, Args...>>>
		constexpr function_ref(Callable & c) noexcept;

		template <auto F>
		static constexpr function_ref to_constant = []()
		{
			function_ref f;
			f.caller = [](void *, Args ... args) -> Ret { return std::invoke(F, args...); };
			return f;
		}();

		constexpr Ret operator () (Args ... args) const;

	private:
		using Base = detail::function_ref_base<function_ptr<Ret(void *, Args...)>>;
	};

	template <typename Ret, typename ... Args>
	struct function_ref<Ret(Args...) noexcept> : public detail::function_ref_base<function_ptr<Ret(void *, Args...) noexcept>>
	{
		constexpr function_ref() noexcept = default;
		constexpr function_ref(std::nullptr_t) noexcept {};

		template <typename Callable, typename = std::enable_if_t<std::is_nothrow_invocable_r_v<Ret, Callable &, Args...>>>
		constexpr function_ref(Callable & c) noexcept;

		template <auto F>
		static constexpr function_ref to_constant = []()
		{
			function_ref f;
			f.caller = [](void *, Args ... args) noexcept -> Ret { return std::invoke(F, args...); };
			return f;
		}();

		constexpr Ret operator () (Args ... args) const noexcept;

	private:
		using Base = detail::function_ref_base<function_ptr<Ret(void *, Args...)>>;
	};

	template <typename F>
	[[nodiscard]] constexpr auto operator == (function_ref<F> f, std::nullptr_t) noexcept -> bool;

	template <typename F>
	[[nodiscard]] constexpr auto operator != (function_ref<F> f, std::nullptr_t) noexcept -> bool;

	template <typename F>
	[[nodiscard]] constexpr auto operator == (std::nullptr_t, function_ref<F> f) noexcept -> bool;

	template <typename F>
	[[nodiscard]] constexpr auto operator != (std::nullptr_t, function_ref<F> f) noexcept -> bool;
	
} // namespace aeh

#include "function_ref.inl"
