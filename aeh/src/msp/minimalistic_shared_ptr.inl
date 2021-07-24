namespace aeh::msp
{

    template <typename T, typename D> constexpr auto operator == (shared_ptr<T, D> const & a, shared_ptr<T, D> const & b) noexcept -> bool { return a.get() == b.get(); }
    template <typename T, typename D> constexpr auto operator <=> (shared_ptr<T, D> const & a, shared_ptr<T, D> const & b) noexcept -> std::strong_ordering { return a.get() <=> b.get(); }
    
    template <typename T, typename D> constexpr auto operator == (shared_ptr<T, D> const & a, std::nullptr_t) noexcept -> bool { return !static_cast<bool>(a); };

    //*********************************************************************************************************************************
    // shared

    template <typename T>
    template <typename ... Args, typename>
    constexpr shared<T>::shared(Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : reference_counter(0)
        , value(std::forward<Args>(args)...)
    {}

    template <typename T>
    template <typename ... Args, typename>
    constexpr shared<T>::shared(shared_always_alive_tag, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : reference_counter(1)
        , value(std::forward<Args>(args)...)
    {}

    //*********************************************************************************************************************************
    // shared_ptr_base

    template <typename T, typename Deleter>
    void shared_ptr_base<T, Deleter>::reset() noexcept
    {
        release_ownership();
        shared_object = nullptr;
    }

    template <typename T, typename Deleter>
    void shared_ptr_base<T, Deleter>::reset(shared_object_type * object_to_take_ownership_of) noexcept
    {
        if (object_to_take_ownership_of != shared_object)
        {
            release_ownership();
            take_ownership_of(object_to_take_ownership_of);
        }
    }

    template <typename T, typename Deleter>
    auto shared_ptr_base<T, Deleter>::use_count() const noexcept -> int
    {
        if (shared_object)
            return shared_object->reference_counter;
        else
            return 0;
    }

    template <typename T, typename Deleter>
    auto shared_ptr_base<T, Deleter>::is_unique() const noexcept -> bool
    {
        return use_count() == 1;
    }

    template <typename T, typename Deleter>
    void shared_ptr_base<T, Deleter>::swap(shared_ptr<T, Deleter> & other) noexcept 
    {
        using std::swap; 
        swap(shared_object, other.shared_object); 

        if constexpr (!std::is_empty_v<Deleter>)
            swap(deleter(), other.deleter());
    }

    template <typename T, typename Deleter>
    void shared_ptr_base<T, Deleter>::take_ownership_of(shared_object_type * object_to_take_ownership_of) noexcept
    {
        shared_object = object_to_take_ownership_of;
        if (object_to_take_ownership_of)
            object_to_take_ownership_of->reference_counter += 1;
    }

    template <typename T, typename Deleter>
    void shared_ptr_base<T, Deleter>::release_ownership() noexcept
    {
        if (shared_object)
        {
            int const count_after_decrement = shared_object->reference_counter -= 1;
            if (count_after_decrement == 0)
                deleter()(shared_object);
        }
    }

    //*********************************************************************************************************************************
    // shared_ptr<T>

    template <typename T, typename Deleter>
    constexpr shared_ptr<T, Deleter>::shared_ptr() noexcept 
        : shared_ptr_base<T, Deleter>(nullptr, Deleter()) 
    {}

    template <typename T, typename Deleter>
    constexpr shared_ptr<T, Deleter>::shared_ptr(std::nullptr_t) noexcept
        : shared_ptr_base<T, Deleter>(nullptr, Deleter()) 
    {}

    template <typename T, typename Deleter>
    shared_ptr<T, Deleter>::shared_ptr(shared<T> * object_to_take_ownership_of) noexcept
        : shared_ptr_base<T, Deleter>(Deleter()) 
    {
        this->take_ownership_of(object_to_take_ownership_of); 
    }


    template <typename T, typename Deleter>
    shared_ptr<T, Deleter>::shared_ptr(shared<T> * object_to_take_ownership_of, Deleter d) noexcept 
        : shared_ptr_base<T, Deleter>(std::move(d)) 
    {
        this->take_ownership_of(object_to_take_ownership_of); 
    }

    template <typename T, typename Deleter>
    shared_ptr<T, Deleter>::shared_ptr(shared_ptr<T> const & other) noexcept
        : shared_ptr_base<T, Deleter>(other.deleter()) 
    { 
        this->take_ownership_of(other.shared_object); 
    }

    template <typename T, typename Deleter>
    shared_ptr<T, Deleter>::shared_ptr(shared_ptr<T> && other) noexcept
        : shared_ptr_base<T, Deleter>(other.shared_object, std::move(other.deleter())) 
    {
        other.shared_object = nullptr; 
    }

    template <typename T, typename Deleter>
    template <typename ... Args>
    auto shared_ptr<T, Deleter>::make_new(Args && ... args) -> shared_ptr<T, Deleter> 
    {
        return shared_ptr<T, Deleter>(new shared<T>(std::forward<Args>(args)...)); 
    }

    template <typename T, typename Deleter>
    template <typename Allocator, typename ... Args>
    auto shared_ptr<T, Deleter>::make_new(Allocator const & alloc, Deleter d, Args && ... args) -> shared_ptr<T, Deleter>
    {
        void * const memory = alloc.allocate(sizeof(shared<T>));
        shared<T> * const shared_object = new (memory) shared<T>(std::forward<Args>(args)...);
        return shared_ptr<T, Deleter>(shared_object, std::move(d));
    }

    template <typename T, typename Deleter>
    auto shared_ptr<T, Deleter>::operator = (shared_ptr<T> const & other) noexcept -> shared_ptr<T> &
    { 
        this->reset(other.shared_object);
        return *this; 
    }

    template <typename T, typename Deleter>
    auto shared_ptr<T, Deleter>::operator = (shared_ptr<T> && other) noexcept -> shared_ptr<T> &
    {
        shared_ptr(std::move(other)).swap(*this);
        return *this; 
    }

    template <typename T, typename Deleter>
    auto shared_ptr<T, Deleter>::operator = (std::nullptr_t) noexcept -> shared_ptr<T> &
    {
        this->reset();
        return *this;
    }

    //*********************************************************************************************************************************
    // shared_ptr<T const>

    template <typename T, typename Deleter>
    constexpr shared_ptr<T const, Deleter>::shared_ptr() noexcept 
        : shared_ptr_base<T const, Deleter>(nullptr, Deleter()) 
    {}

    template <typename T, typename Deleter>
    constexpr shared_ptr<T const, Deleter>::shared_ptr(std::nullptr_t) noexcept
        : shared_ptr_base<T const, Deleter>(nullptr, Deleter()) 
    {}

    template <typename T, typename Deleter>
    shared_ptr<T const, Deleter>::shared_ptr(shared<T> * object_to_take_ownership_of) noexcept
        : shared_ptr_base<T const, Deleter>(Deleter()) 
    {
        this->take_ownership_of(object_to_take_ownership_of); 
    }

    template <typename T, typename Deleter>
    shared_ptr<T const, Deleter>::shared_ptr(shared<T> * object_to_take_ownership_of, Deleter d) noexcept
        : shared_ptr_base<T const, Deleter>(std::move(d)) 
    { 
        this->take_ownership_of(object_to_take_ownership_of); 
    }

    template <typename T, typename Deleter>
    shared_ptr<T const, Deleter>::shared_ptr(shared_ptr<T> const & other) noexcept 
        : shared_ptr_base<T const, Deleter>(other.deleter()) 
    {
        this->take_ownership_of(other.shared_object); 
    }

    template <typename T, typename Deleter>
    shared_ptr<T const, Deleter>::shared_ptr(shared_ptr<T const> const & other) noexcept
        : shared_ptr_base<T const, Deleter>(other.deleter())
    {
        this->take_ownership_of(other.shared_object);
    }

    template <typename T, typename Deleter>
    shared_ptr<T const, Deleter>::shared_ptr(shared_ptr<T> && other) noexcept
        : shared_ptr_base<T const, Deleter>(other.shared_object, std::move(other.deleter())) 
    {
        other.shared_object = nullptr; 
    }

    template <typename T, typename Deleter>
    shared_ptr<T const, Deleter>::shared_ptr(shared_ptr<T const> && other) noexcept
        : shared_ptr_base<T const, Deleter>(other.shared_object, std::move(other.deleter()))
    {
        other.shared_object = nullptr;
    }

    template <typename T, typename Deleter>
    template <typename ... Args>
    auto shared_ptr<T const, Deleter>::make_new(Args && ... args) -> shared_ptr<T const, Deleter>
    {
        return shared_ptr<T const, Deleter>(new shared<T>(std::forward<Args>(args)...)); 
    }

    template <typename T, typename Deleter>
    template <typename Allocator, typename ... Args>
    auto shared_ptr<T const, Deleter>::make_new(Allocator const & alloc, Deleter d, Args && ... args) -> shared_ptr<T const, Deleter>
    {
        void * const memory = alloc.allocate(sizeof(shared<T>));
        shared<T> * const shared_object = new (memory) shared<T>(std::forward<Args>(args)...);
        return shared_ptr<T const, Deleter>(shared_object, std::move(d));
    }

    template <typename T, typename Deleter>
   auto shared_ptr<T const, Deleter>::operator = (shared_ptr<T const> const & other) noexcept -> shared_ptr<T const> &
    {
        this->reset(other.shared_object); 
        return *this; 
    }

    template <typename T, typename Deleter>
   auto shared_ptr<T const, Deleter>::operator = (shared_ptr<T const> && other) noexcept -> shared_ptr<T const> &
    {
        shared_ptr(std::move(other)).swap(*this);
        return *this; 
    }

    template <typename T, typename Deleter>
    auto shared_ptr<T const, Deleter>::operator = (shared_ptr<T> const & other) noexcept -> shared_ptr<T const> &
    {
        this->reset(other.shared_object); 
        return *this; 
    }

    template <typename T, typename Deleter>
    auto shared_ptr<T const, Deleter>::operator = (shared_ptr<T> && other) noexcept -> shared_ptr<T const> &
    {
        shared_ptr(std::move(other)).swap(*this);
        return *this; 
    }

    template <typename T, typename Deleter>
    auto shared_ptr<T const, Deleter>::operator = (std::nullptr_t) noexcept -> shared_ptr<T const> &
    {
        this->reset();
        return *this;
    }

} // namespace aeh::msp
