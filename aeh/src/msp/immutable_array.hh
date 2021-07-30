#pragma once

#include "minimalistic_shared_ptr.hh"
#include "concepts.hh"
#include <memory>
#include <iterator>
#include <cassert>
#include <vector>
#include <span>

namespace aeh::msp
{

    template <typename T>
    struct immutable_array_builder;

    template <typename T>
    struct flexible_immutable_array;

    namespace detail
    {
        struct the_empty_array_buffer_t;
        the_empty_array_buffer_t make_empty_flexible_immutable_array() noexcept;

        template <typename T>
        struct flexible_immutable_array
        {
            flexible_immutable_array() = delete;
            flexible_immutable_array(flexible_immutable_array const &) = delete;
            flexible_immutable_array(flexible_immutable_array &&) = delete;
            auto operator = (flexible_immutable_array const &) -> flexible_immutable_array & = delete;
            auto operator = (flexible_immutable_array &&) -> flexible_immutable_array & = delete;

            ~flexible_immutable_array();

            static auto make_empty()->shared_ptr<flexible_immutable_array>;

            size_t size_;
            T data_[1];
        };
    } // namespace detail

    template <typename T>
    struct immutable_array
    {
        using value_type = T;
        using size_type = size_t;
        using iterator = T const *;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = reverse_iterator;

        immutable_array() noexcept;
        immutable_array(immutable_array const & other) noexcept = default;
        immutable_array(immutable_array && other) noexcept;
        auto operator = (immutable_array const & other) noexcept -> immutable_array & = default;
        auto operator = (immutable_array && other) noexcept -> immutable_array &;

        [[nodiscard]] static auto acquire_ownership_of(shared_ptr<detail::flexible_immutable_array<T>> && ptr_) noexcept -> immutable_array;

        [[nodiscard]] static auto from_ilist(std::initializer_list<T> ilist) -> immutable_array;

        template <std::forward_iterator ForwardIterator> requires std::convertible_to<std::iter_value_t<ForwardIterator>, T>
        [[nodiscard]] static auto copy_of_range(ForwardIterator first, ForwardIterator last) -> immutable_array;

        template <std::ranges::forward_range ForwardRange> requires std::convertible_to<std::ranges::range_value_t<ForwardRange>, T>
        [[nodiscard]] static auto copy_of_range(ForwardRange const & range) -> immutable_array;

        template <std::forward_iterator ForwardIterator> requires std::convertible_to<std::iter_value_t<ForwardIterator>, T>
        [[nodiscard]] static auto move_from_range(ForwardIterator first, ForwardIterator last) -> immutable_array;
        
        template <std::ranges::forward_range ForwardRange> requires std::convertible_to<std::ranges::range_value_t<ForwardRange>, T>
        [[nodiscard]] static auto move_from_range(ForwardRange && range) -> immutable_array;

        template <aeh::invocable_r<std::vector<T>, std::vector<T>> F>
        [[nodiscard]] auto transform(F && f) const -> immutable_array;

        // Permute is a cheaper version of transform that cannot change the size of the array but only takes one allocation, not two.
        template <aeh::invocable_r<void, std::span<T>> F>
        [[nodiscard]] auto permute(F && f) const -> immutable_array;

        [[nodiscard]] auto size() const noexcept -> size_t { return ptr->size_; }
        [[nodiscard]] auto data() const noexcept -> T const * { return ptr->data_; }
        [[nodiscard]] auto empty() const noexcept -> bool { return size() == 0; }
        [[nodiscard]] auto begin() const noexcept -> iterator { return data(); }
        [[nodiscard]] auto end() const noexcept -> iterator { return data() + size(); }
        [[nodiscard]] auto cbegin() const noexcept -> iterator { return begin(); }
        [[nodiscard]] auto cend() const noexcept -> iterator { return end(); }
        [[nodiscard]] auto rbegin() const noexcept -> reverse_iterator { return reverse_iterator(end()); }
        [[nodiscard]] auto rend() const noexcept -> reverse_iterator { return reverse_iterator(begin()); }
        [[nodiscard]] auto crbegin() const noexcept -> const_reverse_iterator { return rbegin(); }
        [[nodiscard]] auto crend() const noexcept -> const_reverse_iterator { return rend(); }
        [[nodiscard]] auto operator [] (size_t i) const noexcept -> T const & { assert(i < size()); return data()[i]; }
        [[nodiscard]] auto nth(size_t i) const noexcept -> iterator { assert(i <= size()); return begin() + i; } // <= instead of < in the assert to allow nth returning the end iterator.
        [[nodiscard]] auto front() const noexcept -> T const & { return data()[0]; }
        [[nodiscard]] auto back() const noexcept -> T const & { return data()[size() - 1]; }
        operator std::span<T const>() const noexcept { return {data(), size()}; }

    private:
        friend struct immutable_array_builder<T>;

        shared_ptr<detail::flexible_immutable_array<T>> ptr;
    };

    template <std::equality_comparable T> [[nodiscard]] auto operator == (immutable_array<T> const & a, immutable_array<T> const & b) noexcept -> bool;
    template <typename T> [[nodiscard]] auto operator <=> (immutable_array<T> const & a, immutable_array<T> const & b) noexcept;

    template <typename T>
    struct immutable_array_builder
    {
        using value_type = T;
        using size_type = size_t;
        using iterator = T *;
        using const_iterator = T const *;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        immutable_array_builder() noexcept = default;

        immutable_array_builder(immutable_array_builder const &) = delete;
        auto operator = (immutable_array_builder const &) -> immutable_array_builder & = delete;

        immutable_array_builder(immutable_array_builder &&) noexcept = default;
        auto operator = (immutable_array_builder &&) noexcept -> immutable_array_builder & = default;

        [[nodiscard]] static auto with_size(size_t size) -> immutable_array_builder;

        [[nodiscard]] static auto from_ilist(std::initializer_list<T> ilist) -> immutable_array_builder;

        template <typename ForwardIterator>
        [[nodiscard]] static auto copy_of_range(ForwardIterator first, ForwardIterator last) -> immutable_array_builder;

        template <typename ForwardIterator>
        [[nodiscard]] static auto move_from_range(ForwardIterator first, ForwardIterator last) -> immutable_array_builder;

        [[nodiscard]] auto size() const noexcept -> size_t { return array_being_built.ptr->size_; }
        [[nodiscard]] auto data() noexcept -> T * { return array_being_built.ptr->data_; }
        [[nodiscard]] auto data() const noexcept -> T const * { return array_being_built.ptr->data_; }
        [[nodiscard]] auto begin() noexcept -> iterator { return data(); }
        [[nodiscard]] auto end() noexcept -> iterator { return data() + size(); }
        [[nodiscard]] auto begin() const noexcept -> const_iterator { return cbegin(); }
        [[nodiscard]] auto cbegin() const noexcept -> const_iterator { return data(); }
        [[nodiscard]] auto end() const noexcept -> const_iterator { return cend(); }
        [[nodiscard]] auto cend() const noexcept -> const_iterator { return data() + size(); }
        [[nodiscard]] auto rbegin() noexcept -> reverse_iterator { return reverse_iterator(end()); }
        [[nodiscard]] auto rend() noexcept -> reverse_iterator { return reverse_iterator(begin()); }
        [[nodiscard]] auto rbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator(end()); }
        [[nodiscard]] auto rend() const noexcept -> const_reverse_iterator { return const_reverse_iterator(begin()); }
        [[nodiscard]] auto crbegin() const noexcept -> const_reverse_iterator { return rbegin(); }
        [[nodiscard]] auto crend() const noexcept -> const_reverse_iterator { return rend(); }
        [[nodiscard]] auto operator [] (size_t i) noexcept -> T & { assert(i < size()); return data()[i]; }
        [[nodiscard]] auto operator [] (size_t i) const noexcept -> T const & { assert(i < size()); return data()[i]; } 
        [[nodiscard]] auto nth(size_t i) noexcept -> iterator { assert(i <= size()); return begin() + i; }              // <= instead of < in the assert to allow nth returning the end iterator.
        [[nodiscard]] auto nth(size_t i) const noexcept -> const_iterator { assert(i <= size()); return begin() + i; }  // <= instead of < in the assert to allow nth returning the end iterator.
        [[nodiscard]] auto front() noexcept -> T & { return data()[0]; }
        [[nodiscard]] auto front() const noexcept -> T const & { return data()[0]; }
        [[nodiscard]] auto back() noexcept -> T & { return data()[size() - 1]; }
        [[nodiscard]] auto back() const noexcept -> T const & { return data()[size() - 1]; }

        operator std::span<T>() noexcept { return {data(), size()}; }
        operator std::span<T const>() const noexcept { return {data(), size()}; }

        [[nodiscard]] auto finish() noexcept -> immutable_array<T> { return std::move(array_being_built); }
        [[nodiscard]] auto has_finished() const noexcept -> bool { return array_being_built.empty(); }

    private:
        explicit immutable_array_builder(immutable_array<T> array) noexcept : array_being_built(std::move(array)) {}
        immutable_array<T> array_being_built;
    };

} // namespace aeh::msp

#include "immutable_array.inl"
