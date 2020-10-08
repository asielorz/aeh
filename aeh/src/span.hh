#pragma once

#include "priority.hh"
#include <type_traits>
#include <initializer_list>

namespace aeh
{

	template <typename T>
	struct span;

	namespace detail
	{
		template <typename T>
		struct span_impl
		{
			using value_type = T;
			using size_type = size_t;
			using iterator = T *;
			using reverse_iterator = std::reverse_iterator<iterator>;

			constexpr span_impl(T data_[], size_t size_) noexcept : data_array(data_), data_size(size_) {}

			constexpr auto operator [] (size_t i) const noexcept -> T & { return data_array[i]; }
			constexpr auto data() const noexcept -> T * { return data_array; }
			constexpr auto size() const noexcept -> size_t { return data_size; }
			constexpr auto empty() const noexcept -> bool { return size() == 0; }
			constexpr auto begin() const noexcept -> T * { return data(); }
			constexpr auto end() const noexcept -> T * { return data() + size(); }
			constexpr auto rbegin() const noexcept -> reverse_iterator { return reverse_iterator(end()); }
			constexpr auto rend() const noexcept -> reverse_iterator { return reverse_iterator(begin()); }
			constexpr auto front() const noexcept -> T & { return data()[0]; }
			constexpr auto back() const noexcept -> T & { return data()[size() - 1]; }
			constexpr auto subspan(size_t offset, size_t count) const noexcept -> span<T> { return span<T>(data() + offset, std::min(count, size() - offset)); }

		protected:
			T * data_array;
			size_t data_size;
		};

		template <typename T, typename ValueType, typename ResultType = decltype(std::declval<T>().data())>
		constexpr bool has_data(priority_tag<1>) { return std::is_convertible_v<ResultType, ValueType *>; }
		template <typename T, typename ValueType>
		constexpr bool has_data(priority_tag<0>) { return false; }

		template <typename T, typename ResultType = decltype(std::declval<T>().size())>
		constexpr bool has_size(priority_tag<1>) { return std::is_same_v<ResultType, size_t>; }
		template <typename T, typename ValueType>
		constexpr bool has_size(priority_tag<0>) { return false; }

	} // namespace detail

	template <typename T, typename ValueType>
	struct is_array_like
	{
		static constexpr bool value = detail::has_data<T, ValueType>(priority_tag_v<1>) && detail::has_size<T, ValueType>(priority_tag_v<1>);
	};
	template <typename T, typename ValueType>
	constexpr bool is_array_like_v = is_array_like<T, ValueType>::value;

	template <typename T>
	struct span : public ::aeh::detail::span_impl<T>
	{
		constexpr span() noexcept : detail::span_impl<T>(nullptr, 0) {}
		constexpr span(T data_[], size_t size_) noexcept : detail::span_impl<T>(data_, size_) {}
		template <size_t N>	constexpr span(T(&array)[N]) noexcept : detail::span_impl<T>(array, N) {}

		template <typename ArrayLike, typename = std::enable_if_t<is_array_like_v<ArrayLike, T>>>
		constexpr span(ArrayLike && array) noexcept : detail::span_impl<T>(array.data(), array.size()) {}
	};

	template <typename T>
	struct span<T const> : public ::aeh::detail::span_impl<T const>
	{
		constexpr span() noexcept : detail::span_impl<T const>(nullptr, 0) {}
		constexpr span(T const data_[], size_t size_) noexcept : detail::span_impl<T const>(data_, size_) {}
		constexpr span(span<T> s) noexcept : detail::span_impl<T const>(s.data(), s.size()) {}
		template <size_t N>	constexpr span(T(&data_)[N]) noexcept : detail::span_impl<T const>(data_, N) {}
		template <size_t N>	constexpr span(T const(&data_)[N]) noexcept : detail::span_impl<T const>(data_, N) {}
		constexpr span(std::initializer_list<T> ilist) noexcept : detail::span_impl<T const>(std::data(ilist), std::size(ilist)) {}

		template <typename ArrayLike, typename = std::enable_if_t<is_array_like_v<ArrayLike, T const>>>
		constexpr span(ArrayLike && array) noexcept : detail::span_impl<T>(array.data(), array.size()) {}
	};

} // namespace aeh
