#pragma once

#include <optional>
#include <concepts>

namespace aeh
{

	template <typename T> struct remove_optional {};
	template <typename T> struct remove_optional<std::optional<T>>
	{
		using type = T;
	};
	template <typename T> using dereference_t = std::remove_cvref_t<decltype(*std::declval<T>())>;

	struct generator_sentinel_t {};
	constexpr generator_sentinel_t generator_sentinel;

	template <std::invocable<> F>
	struct generator_iterator
	{
		using optional_type = std::invoke_result_t<F>;
		using value_type = dereference_t <optional_type>;
		using iterator_category = std::forward_iterator_tag;
		static constexpr bool is_noexcept = std::is_nothrow_invocable_v<F>;

		constexpr explicit generator_iterator(F const & function) noexcept(is_noexcept && std::is_nothrow_copy_constructible_v<F>);
		constexpr explicit generator_iterator(F && function) noexcept(is_noexcept);

		[[nodiscard]] constexpr auto operator * () noexcept -> value_type &;
		constexpr auto operator -> () noexcept -> value_type *;

		[[nodiscard]] constexpr auto operator == (generator_sentinel_t) const noexcept -> bool;

		constexpr auto operator ++ () noexcept(is_noexcept) -> generator_iterator &;
		constexpr auto operator ++ (int) noexcept(is_noexcept) -> generator_iterator;

	private:
		F generator_function;
		optional_type last_generated;
	};

	template <std::invocable<> F>
	struct generator
	{
		using optional_type = std::invoke_result_t<F>;
		using value_type = dereference_t<optional_type>;
		static constexpr bool is_noexcept = std::is_nothrow_invocable_v<F>;

		constexpr explicit generator(F const & function) noexcept(std::is_nothrow_copy_constructible_v<F>);
		constexpr explicit generator(F && function) noexcept;

		[[nodiscard]] constexpr auto begin() const & noexcept(std::is_nothrow_copy_constructible_v<F>) -> generator_iterator<F>;
		[[nodiscard]] constexpr auto begin() && noexcept -> generator_iterator<F>;
		[[nodiscard]] constexpr auto end() const noexcept -> generator_sentinel_t;

		constexpr auto operator () () noexcept(is_noexcept) -> std::optional<value_type>;

		constexpr auto generator_function() const & noexcept -> F const &;
		constexpr auto generator_function() && noexcept -> F &&;

	private:
		F generator_function_;
	};

} // namespace aeh

#include "generator.inl"
