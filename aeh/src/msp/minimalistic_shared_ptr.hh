#pragma once

#include <atomic>

// Non intrusive reference counter type with no weak pointer, no shared pointer to subobject,
// no casting and no taking ownership of already allocated objects. 
// In exchange, the shared pointer object contains a single pointer and the control block
// is always allocated with the object and contains a single integer.
namespace aeh::msp
{
    struct default_delete
    {
        template <typename T> void operator () (T * p) const { delete p; }
    };

    struct shared_always_alive_tag {};

    template <typename T>
    struct shared
    {

        template <typename ... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>> 
        constexpr shared(Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>);

        template <typename ... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
        constexpr shared(shared_always_alive_tag, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>);


        constexpr shared(shared<T> const &) noexcept(std::is_nothrow_copy_constructible_v<T>) = default;
        constexpr shared(shared<T> &&) noexcept(std::is_nothrow_move_constructible_v<T>) = default;

        std::atomic<int> reference_counter;
        T value;
    };

    template <typename T, typename Deleter>
    struct shared_ptr;

    template <typename T, typename Deleter = default_delete>
    struct shared_ptr_base
    {
        using shared_object_type = shared<std::remove_const_t<T>>;

        ~shared_ptr_base() { release_ownership(); }

        void reset() noexcept;
        void reset(shared_object_type * object_to_take_ownership_of) noexcept;

        [[nodiscard]] constexpr auto get() const noexcept -> T * { return &shared_object->value; }
        [[nodiscard]] auto operator * () const noexcept -> T & { return *get(); }
        auto operator -> () const noexcept -> T * { return get(); }

        [[nodiscard]] auto use_count() const noexcept -> int;
        [[nodiscard]] auto is_unique() const noexcept -> bool;
        constexpr explicit operator bool () const noexcept { return shared_object != nullptr; }

        friend struct shared_ptr<T const, Deleter>;

    protected:
        constexpr shared_ptr_base(Deleter d) noexcept : deleter(std::move(d)) {}
        constexpr shared_ptr_base(shared_object_type * object_to_take_ownership_of, Deleter d) noexcept
            : shared_object(object_to_take_ownership_of) 
            , deleter(std::move(d))
        {}
        void take_ownership_of(shared_object_type * object_to_take_ownership_of) noexcept;
        void release_ownership() noexcept;

        shared_object_type * shared_object;
        Deleter deleter;
    };

    template <typename T, typename Deleter = default_delete>
    struct shared_ptr : shared_ptr_base<T, Deleter>
    {
        using element_type = T;
        using deleter_type = Deleter;

        constexpr shared_ptr() noexcept;
        constexpr shared_ptr(std::nullptr_t) noexcept;
        explicit shared_ptr(shared<T> * object_to_take_ownership_of) noexcept;
        explicit shared_ptr(shared<T> * object_to_take_ownership_of, Deleter d) noexcept;
        shared_ptr(shared_ptr<T> const & other) noexcept;
        shared_ptr(shared_ptr<T> && other) noexcept;

        template <typename ... Args> 
        [[nodiscard]] static auto make_new(Args && ... args) -> shared_ptr<T, Deleter>;

        template <typename Allocator, typename ... Args> 
        [[nodiscard]] static auto make_new(Allocator const & alloc, Deleter d, Args && ... args) -> shared_ptr<T, Deleter>;

        auto operator = (shared_ptr<T> const & other) noexcept -> shared_ptr<T> &;
        auto operator = (shared_ptr<T> && other) noexcept -> shared_ptr<T> &;
    };

    template <typename T, typename Deleter>
    struct shared_ptr<T const, Deleter> : shared_ptr_base<T const, Deleter>
    {
        using element_type = T const;
        using deleter_type = Deleter;

        constexpr shared_ptr() noexcept;
        constexpr shared_ptr(std::nullptr_t) noexcept;
        explicit shared_ptr(shared<T> * object_to_take_ownership_of) noexcept;
        explicit shared_ptr(shared<T> * object_to_take_ownership_of, Deleter d) noexcept;
        shared_ptr(shared_ptr<T> const & other) noexcept ;
        shared_ptr(shared_ptr<T> && other) noexcept;

        template <typename ... Args> 
        [[nodiscard]] static auto make_new(Args && ... args) -> shared_ptr<T const, Deleter>;

        template <typename Allocator, typename ... Args> 
        [[nodiscard]] static auto make_new(Allocator const & alloc, Deleter d, Args && ... args) -> shared_ptr<T const, Deleter>;

        auto operator = (shared_ptr<T const> const & other) noexcept -> shared_ptr<T const> &;
        auto operator = (shared_ptr<T const> && other) noexcept -> shared_ptr<T const> &;
        auto operator = (shared_ptr<T> const & other) noexcept -> shared_ptr<T const> &;
        auto operator = (shared_ptr<T> && other) noexcept -> shared_ptr<T const> &;
    };

    template <typename T, typename D>  [[nodiscard]] constexpr bool operator == (shared_ptr<T, D> const & a, shared_ptr<T, D> const & b) noexcept;
    template <typename T, typename D>  [[nodiscard]] constexpr bool operator != (shared_ptr<T, D> const & a, shared_ptr<T, D> const & b) noexcept;
    template <typename T, typename D>  [[nodiscard]] constexpr bool operator <  (shared_ptr<T, D> const & a, shared_ptr<T, D> const & b) noexcept;
    template <typename T, typename D>  [[nodiscard]] constexpr bool operator <= (shared_ptr<T, D> const & a, shared_ptr<T, D> const & b) noexcept;
    template <typename T, typename D>  [[nodiscard]] constexpr bool operator >  (shared_ptr<T, D> const & a, shared_ptr<T, D> const & b) noexcept;
    template <typename T, typename D>  [[nodiscard]] constexpr bool operator >= (shared_ptr<T, D> const & a, shared_ptr<T, D> const & b) noexcept;

} // namespace aeh::msp

#include "minimalistic_shared_ptr.inl"
