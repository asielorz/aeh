#pragma once

#include "span.hh"
#include "minimalistic_shared_ptr.hh"
#include <memory>
#include <iterator>
#include <cassert>
#include <vector>

namespace aeh::msp
{

    template <typename T>
    struct flexible_immutable_array_builder;

    template <typename T>
    struct flexible_immutable_array;

    namespace detail
    {
        struct the_empty_array_buffer_t;
        the_empty_array_buffer_t make_empty_flexible_immutable_array() noexcept;
    }

    template <typename T>
    struct flexible_immutable_array
    {
        using value_type = T;
        using size_type = size_t;
        using iterator = T const *;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = reverse_iterator;

        flexible_immutable_array() = delete;
        flexible_immutable_array(flexible_immutable_array const &) = delete;
        flexible_immutable_array(flexible_immutable_array &&) = delete;
        flexible_immutable_array & operator = (flexible_immutable_array const &) = delete;
        flexible_immutable_array & operator = (flexible_immutable_array &&) = delete;

        ~flexible_immutable_array();

        static auto make_empty() -> shared_ptr<flexible_immutable_array>;
        [[nodiscard]] static auto from_ilist(std::initializer_list<T> ilist) -> shared_ptr<flexible_immutable_array>;

        template <typename ForwardIterator>
        [[nodiscard]] static auto copy_of_range(ForwardIterator first, ForwardIterator last) -> shared_ptr<flexible_immutable_array>;

        template <typename ForwardIterator>
        [[nodiscard]] static auto move_from_range(ForwardIterator first, ForwardIterator last) -> shared_ptr<flexible_immutable_array>;

        template <typename F>
        [[nodiscard]] auto transform(F && f) const -> shared_ptr<flexible_immutable_array>;

        template <typename F>
        [[nodiscard]] auto permute(F && f) const -> shared_ptr<flexible_immutable_array>;

        [[nodiscard]] auto size() const noexcept -> size_t { return size_; }
        [[nodiscard]] auto data() const noexcept -> T const * { return data_; }
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
        [[nodiscard]] auto front() const noexcept -> T const & { return data()[0]; }
        [[nodiscard]] auto back() const noexcept -> T const & { return data()[size() - 1]; }
        operator span<T const>() const noexcept { return { data(), size() }; }

        friend struct flexible_immutable_array_builder<T>;
        friend detail::the_empty_array_buffer_t detail::make_empty_flexible_immutable_array() noexcept;

    private:
        size_t size_;
        T data_[1];
    };

    template <typename T> [[nodiscard]] bool operator == (flexible_immutable_array<T> const & a, flexible_immutable_array<T> const & b) noexcept;
    template <typename T> [[nodiscard]] bool operator != (flexible_immutable_array<T> const & a, flexible_immutable_array<T> const & b) noexcept;

    template <typename T>
    struct flexible_immutable_array_builder
    {
        using value_type = T;
        using size_type = size_t;
        using iterator = T *;
        using const_iterator = T const *;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        [[nodiscard]] static auto with_size(size_t size)->flexible_immutable_array_builder;

        template <typename ForwardIterator>
        [[nodiscard]] static auto copy_of_range(ForwardIterator first, ForwardIterator last)->flexible_immutable_array_builder;

        template <typename ForwardIterator>
        [[nodiscard]] static auto move_from_range(ForwardIterator first, ForwardIterator last)->flexible_immutable_array_builder;

        [[nodiscard]] auto size() const noexcept -> size_t { return array_being_built->size_; }
        [[nodiscard]] auto data() noexcept -> T * { return array_being_built->data_; }
        [[nodiscard]] auto data() const noexcept -> T const * { return array_being_built->data_; }
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
        [[nodiscard]] auto front() noexcept -> T & { return data()[0]; }
        [[nodiscard]] auto front() const noexcept -> T const & { return data()[0]; }
        [[nodiscard]] auto back() noexcept -> T & { return data()[size() - 1]; }
        [[nodiscard]] auto back() const noexcept -> T const & { return data()[size() - 1]; }

        operator span<T>() noexcept { return { data(), size() }; }
        operator span<T const>() const noexcept { return { data(), size() }; }

        [[nodiscard]] auto finish() noexcept -> shared_ptr<flexible_immutable_array<T>> { return std::move(array_being_built); }
        [[nodiscard]] bool has_finished() const noexcept { return array_being_built == nullptr; }

    private:
        explicit flexible_immutable_array_builder(shared_ptr<flexible_immutable_array<T>> array) : array_being_built(std::move(array)) {}
        shared_ptr<flexible_immutable_array<T>> array_being_built;
    };

} // namespace aeh::msp

#include "flexible_immutable_array.inl"
