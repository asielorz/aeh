#pragma once

#include "minimalistic_shared_ptr.hh"

namespace aeh::msp
{

    template <typename T>
    struct cow
    {
        cow() noexcept = default;
        cow(cow const &) noexcept = default;
        cow(cow &&) noexcept;
        cow & operator = (cow const &) noexcept = default;
        cow & operator = (cow &&) noexcept;
        explicit cow(T const & t);
        explicit cow(T && t);

        [[nodiscard]] auto operator * () -> T &;
        [[nodiscard]] auto operator * () const noexcept -> T const &;
        auto operator -> () -> T *;
        auto operator -> () const noexcept -> T const *;

    private:
        shared_ptr<T> pointer = default_pointer();

        static shared<T> default_object;
        static shared_ptr<T> default_pointer() noexcept;
    };

    template <typename T> auto operator == (cow<T> const & a, cow<T> const & b) noexcept -> bool;
    template <typename T> auto operator != (cow<T> const & a, cow<T> const & b) noexcept -> bool;

} // namespace aeh::msp

#include "copy_on_write.inl"
