#pragma once

#include <concepts>
#include <span>
#include <cassert>
#include <memory>

namespace aeh
{

	template <typename Alloc, typename T>
	concept std_allocator =
		std::copyable<Alloc> &&
		requires(Alloc alloc, T* p, size_t n) {
			typename Alloc::value_type;
			{ alloc.allocate(n) } -> std::same_as<T *>;
			alloc.deallocate(p, n);
	};

	template <std::move_constructible T, std_allocator<T> Allocator = std::allocator<T>>
	struct ring
	{
		using value_type = T;
		using reference = T&;
		using const_reference = T const&;
		using size_type = size_t;
		using allocator_type = Allocator;

		constexpr ring() noexcept = default;
		constexpr explicit ring(Allocator allocator) noexcept;
		constexpr ~ring();

		constexpr ring(const ring& other) requires std::copy_constructible<T>;
		constexpr ring(ring&& other) noexcept;
		constexpr auto operator = (const ring& other) -> ring& requires std::copy_constructible<T>;
		constexpr auto operator = (ring&& other) noexcept -> ring&;

		constexpr auto empty() const noexcept -> bool;
		constexpr auto size() const noexcept -> size_t;
		constexpr auto capacity() const noexcept -> size_t;

		constexpr auto front() noexcept -> T&;
		constexpr auto front() const noexcept -> const T&;
		constexpr auto back() noexcept -> T&;
		constexpr auto back() const noexcept -> const T&;

		constexpr auto push_back(const T& value) -> T& requires std::copy_constructible<T>;
		constexpr auto push_back(T&& value) -> T&;

		template <typename ... Args> requires std::constructible_from<T, Args...>
		constexpr auto emplace_back(Args&& ... args) -> T&;

		constexpr void pop_front() noexcept;

		constexpr void clear() noexcept;
		constexpr void reset() noexcept;

		constexpr void reserve(size_t new_capacity);
		constexpr void shrink_to_fit();

		constexpr auto get_allocator() const noexcept -> Allocator;

	private:
		constexpr void change_capacity_to(size_t new_capacity);
		constexpr std::pair<std::span<T>, std::span<T>> data() noexcept;
		constexpr std::pair<std::span<T const>, std::span<T const>> data() const noexcept;

		T* buffer_ = nullptr;
		size_t capacity_ = 0;
		size_t first_ = 0;
		size_t size_ = 0;
		[[no_unique_address]] Allocator allocator_;
	};

}

#include "ring.inl"
