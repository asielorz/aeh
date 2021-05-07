namespace aeh::msp
{

    template <typename T> shared<T> cow<T>::default_object = shared<T>(msp::shared_always_alive_tag());

    template <typename T>
    cow<T>::cow(cow<T> && other) noexcept
        : pointer(std::move(other.pointer))
    {
        other.pointer = default_pointer();
    }

    template <typename T>
    cow<T> & cow<T>::operator = (cow<T> && other) noexcept
    {
        pointer = std::move(other.pointer);
        other.pointer = default_pointer();
        return *this;
    }

    template <typename T>
    cow<T>::cow(T const & t)
        : pointer(shared_ptr<T>::make_new(t))
    {}

    template <typename T>
    cow<T>::cow(T && t)
        : pointer(shared_ptr<T>::make_new(std::move(t)))
    {}

    template <typename T>
    auto cow<T>::lock() -> T &
    {
        if (!pointer.is_unique())
            pointer = shared_ptr<T>::make_new(*pointer);

        return *pointer;
    }

    template <typename T>
    auto cow<T>::read() const noexcept -> T const &
    {
        return *pointer;
    }

    template <typename T>
    auto cow<T>::operator * () -> T &
    {
        return lock();
    }

    template <typename T>
    auto cow<T>::operator * () const noexcept -> T const &
    {
        return read();
    }

    template <typename T>
    auto cow<T>::operator -> () -> T *
    {
        if (!pointer.is_unique())
            pointer = shared_ptr<T>::make_new(*pointer);

        return pointer.get();
    }

    template <typename T>
    auto cow<T>::operator -> () const noexcept -> T const *
    {
        return pointer.get();
    }

    template <typename T>
    shared_ptr<T> cow<T>::default_pointer() noexcept
    {
        return shared_ptr<T>(&default_object);
    }

    template <typename T>
    auto operator == (cow<T> const & a, cow<T> const & b) noexcept -> bool
    {
        return &(*a) == &(*b) || *a == *b;
    }

} // namespace aeh::msp
