#pragma once

#include "debug/assert.hh"
#include <span>
#include <type_traits> // aligned_storage
#include <new> // launder
#include <iterator> // reverse_iterator
#include <algorithm> // rotate
#include <memory> // destroy
#include <limits>

namespace aeh
{

	template <typename T, size_t Capacity>
	struct fixed_capacity_vector
	{
		using value_type = T;
		using iterator = T *;
		using const_iterator = T const *;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using size_type = size_t;

		constexpr fixed_capacity_vector() noexcept = default;
		constexpr fixed_capacity_vector(std::initializer_list<T> ilist) noexcept;
		
		constexpr ~fixed_capacity_vector() requires(std::is_trivially_destructible_v<T>) = default;
		constexpr ~fixed_capacity_vector() requires(!std::is_trivially_destructible_v<T>);
		
		constexpr fixed_capacity_vector(fixed_capacity_vector const & other) noexcept requires(std::is_trivially_copy_constructible_v<T>) = default;
		constexpr fixed_capacity_vector(fixed_capacity_vector const & other) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(!std::is_trivially_copy_constructible_v<T>);

		constexpr fixed_capacity_vector(fixed_capacity_vector && other) noexcept requires(std::is_trivially_move_constructible_v<T>) = default;
		constexpr fixed_capacity_vector(fixed_capacity_vector && other) noexcept(std::is_nothrow_move_constructible_v<T>) requires(!std::is_trivially_move_constructible_v<T>);

		constexpr auto operator = (fixed_capacity_vector const & other) noexcept -> fixed_capacity_vector & requires(std::is_trivially_copy_constructible_v<T>) = default;
		constexpr auto operator = (fixed_capacity_vector const & other) noexcept(std::is_nothrow_copy_constructible_v<T>) -> fixed_capacity_vector & requires(!std::is_trivially_copy_constructible_v<T>);

		constexpr auto operator = (fixed_capacity_vector && other) noexcept -> fixed_capacity_vector & requires(std::is_trivially_move_constructible_v<T>) = default;
		constexpr auto operator = (fixed_capacity_vector && other) noexcept(std::is_nothrow_move_constructible_v<T>) -> fixed_capacity_vector & requires(!std::is_trivially_move_constructible_v<T>);

		[[nodiscard]] constexpr auto size() const noexcept -> size_t;
		[[nodiscard]] constexpr auto ssize() const noexcept -> ptrdiff_t;
		[[nodiscard]] constexpr auto int_size() const noexcept -> int requires(Capacity <= std::numeric_limits<int>::max());
		[[nodiscard]] constexpr auto empty() const noexcept -> bool;
		[[nodiscard]] static constexpr auto capacity() noexcept -> size_t;
		[[nodiscard]] static constexpr auto max_size() noexcept -> size_t;
		[[nodiscard]] constexpr auto data() noexcept -> T *;
		[[nodiscard]] constexpr auto data() const noexcept -> T const *;
		[[nodiscard]] constexpr auto operator [] (size_t i) noexcept -> T &;
		[[nodiscard]] constexpr auto operator [] (size_t i) const noexcept -> T const &;
		[[nodiscard]] constexpr auto front() noexcept -> T &;
		[[nodiscard]] constexpr auto front() const noexcept -> T const &;
		[[nodiscard]] constexpr auto back() noexcept -> T &;
		[[nodiscard]] constexpr auto back() const noexcept -> T const &;

		[[nodiscard]] constexpr auto begin() noexcept -> iterator;
		[[nodiscard]] constexpr auto end() noexcept -> iterator;
		[[nodiscard]] constexpr auto begin() const noexcept -> const_iterator;
		[[nodiscard]] constexpr auto end() const noexcept -> const_iterator;
		[[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator;
		[[nodiscard]] constexpr auto cend() const noexcept -> const_iterator;

		[[nodiscard]] constexpr auto rbegin() noexcept -> reverse_iterator;
		[[nodiscard]] constexpr auto rend() noexcept -> reverse_iterator;
		[[nodiscard]] constexpr auto rbegin() const noexcept -> const_reverse_iterator;
		[[nodiscard]] constexpr auto rend() const noexcept -> const_reverse_iterator;
		[[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator;
		[[nodiscard]] constexpr auto crend() const noexcept -> const_reverse_iterator;

		[[nodiscard]] constexpr auto nth(size_t i) noexcept -> iterator;
		[[nodiscard]] constexpr auto nth(size_t i) const noexcept -> const_iterator;

		constexpr operator std::span<T>() noexcept;
		constexpr operator std::span<T const>() const noexcept;

		constexpr auto clear() noexcept -> void;
		constexpr auto push_back(T const & t) noexcept(std::is_nothrow_copy_constructible_v<T>) -> T &;
		constexpr auto push_back(T && t) noexcept(std::is_nothrow_move_constructible_v<T>)->T &;
		template <typename ... Args> constexpr auto emplace_back(Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) -> T &;
		constexpr auto pop_back() noexcept -> void;
		constexpr auto resize(size_t new_size) noexcept -> void;
		constexpr auto resize(size_t new_size, T const & value) noexcept -> void;
		constexpr auto erase(const_iterator pos) noexcept -> iterator;
		constexpr auto erase(const_iterator first, const_iterator last) noexcept -> const_iterator;

	private:
		std::aligned_storage_t<sizeof(T) * Capacity, alignof(T)> buffer = {0};
		size_t size_ = 0;
	};

	template <typename T, size_t Capacity>
	[[nodiscard]] constexpr auto operator == (fixed_capacity_vector<T, Capacity> const & a, fixed_capacity_vector<T, Capacity> const & b) noexcept -> bool;

	template <typename T, size_t Capacity>
	[[nodiscard]] constexpr auto operator <=> (fixed_capacity_vector<T, Capacity> const & a, fixed_capacity_vector<T, Capacity> const & b) noexcept;

} // namespace aeh

#include "fixed_capacity_vector.inl"
