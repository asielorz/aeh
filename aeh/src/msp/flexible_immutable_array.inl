namespace aeh::msp
{

    namespace detail
    {

        struct the_empty_array_buffer_t
        {
            std::aligned_storage_t<sizeof(shared<flexible_immutable_array<int>>), alignof(shared<flexible_immutable_array<int>>)> buffer;
        };
        the_empty_array_buffer_t make_empty_flexible_immutable_array() noexcept
        {
            the_empty_array_buffer_t buffer;
            auto const new_array = std::launder(reinterpret_cast<shared<flexible_immutable_array<int>> *>(&buffer));
            new_array->reference_counter = 1; // Must never run out of references since it cannot be deleted because it's a global.
            new_array->value.size_ = 0;
            return buffer;
        }
        the_empty_array_buffer_t the_empty_array_buffer = make_empty_flexible_immutable_array();
        shared<flexible_immutable_array<int>> & the_empty_array = *std::launder(reinterpret_cast<shared<flexible_immutable_array<int>> *>(&the_empty_array_buffer));

    } // namespace detail

    template <typename T>
    flexible_immutable_array<T>::~flexible_immutable_array()
    {
        size_t const n = size();
        T * elements = data_;
        std::destroy_n(std::reverse_iterator<T *>(elements + n), n - 1);
    }

    template <typename T>
    auto flexible_immutable_array<T>::make_empty() -> shared_ptr<flexible_immutable_array>
    {
        return shared_ptr<flexible_immutable_array>(reinterpret_cast<shared<flexible_immutable_array> *>(&detail::the_empty_array));
    }

    template <typename T>
    auto flexible_immutable_array<T>::from_ilist(std::initializer_list<T> ilist) -> shared_ptr<flexible_immutable_array>
    {
        return copy_of_range(ilist.begin(), ilist.end());
    }

    template <typename T>
    template <typename ForwardIterator>
    auto flexible_immutable_array<T>::copy_of_range(ForwardIterator first, ForwardIterator last) -> shared_ptr<flexible_immutable_array>
    {
        size_t const size = std::distance(first, last);

        if (size == 0)
            return make_empty();

        size_t const bytes_needed = sizeof(msp::shared<flexible_immutable_array>) + sizeof(T) * (size - 1);
        auto new_array = static_cast<msp::shared<flexible_immutable_array> *>(::operator new(bytes_needed));
        new_array->reference_counter = 0;
        new_array->value.size_ = size;
        std::uninitialized_copy(first, last, static_cast<T *>(new_array->value.data_));
        return shared_ptr<flexible_immutable_array>(new_array);
    }

    template <typename T>
    template <typename ForwardIterator>
    auto flexible_immutable_array<T>::move_from_range(ForwardIterator first, ForwardIterator last) -> shared_ptr<flexible_immutable_array>
    {
        size_t const size = std::distance(first, last);

        if (size == 0)
            return make_empty();

        size_t const bytes_needed = sizeof(shared<flexible_immutable_array>) + sizeof(T) * (size - 1);
        auto new_array = static_cast<shared<flexible_immutable_array> *>(::operator new(bytes_needed));
        new_array->reference_counter = 0;
        new_array->value.size_ = size;
        std::uninitialized_move(first, last, static_cast<T *>(new_array->value.data_));
        return shared_ptr<flexible_immutable_array>(new_array);
    }

    template <typename T>
    template <typename F>
    auto flexible_immutable_array<T>::transform(F && f) const -> shared_ptr<flexible_immutable_array>
    {
        std::vector<T> contents;
        size_t const n = size();
        contents.reserve(n * 2);
        contents.assign(begin(), end());
        auto transformed_contents = std::forward<F>(f)(std::move(contents));
        return move_from_range(transformed_contents.begin(), transformed_contents.end());
    }

    template <typename T>
    template <typename F>
    auto flexible_immutable_array<T>::permute(F && f) const -> shared_ptr<flexible_immutable_array>
    {
        auto builder = flexible_immutable_array_builder<T>::copy_of_range(begin(), end());
        std::forward<F>(f)(span<T>(builder.data(), builder.size()));
        return builder.finish();
    }

    template <typename T>
    bool operator == (flexible_immutable_array<T> const & a, flexible_immutable_array<T> const & b) noexcept
    {
        return (&a == &b) || ((a.size() == b.size()) && std::equal(a.begin(), a.end(), b.begin(), b.end()));
    }

    template <typename T>
    bool operator != (flexible_immutable_array<T> const & a, flexible_immutable_array<T> const & b) noexcept
    {
        return !(a == b);
    }

    template <typename T>
    auto flexible_immutable_array_builder<T>::with_size(size_t size) -> flexible_immutable_array_builder
    {
        assert(size > 0);
        size_t const bytes_needed = sizeof(shared<flexible_immutable_array>) + sizeof(T) * (size - 1);
        auto new_array = static_cast<shared<flexible_immutable_array> *>(::operator new(bytes_needed));
        new_array->reference_counter = 0;
        new_array->value.size_ = size;
        std::uninitialized_default_construct_n(static_cast<T *>(new_array->value.data_), size);
        return flexible_immutable_array_builder(shared_ptr<flexible_immutable_array<T>>(new_array));
    }

    template <typename T>
    template <typename ForwardIterator>
    auto flexible_immutable_array_builder<T>::copy_of_range(ForwardIterator first, ForwardIterator last) -> flexible_immutable_array_builder
    {
        assert(std::distance(first, last) > 0);
        return flexible_immutable_array_builder(flexible_immutable_array<T>::copy_of_range(first, last));
    }

    template <typename T>
    template <typename ForwardIterator>
    auto flexible_immutable_array_builder<T>::move_from_range(ForwardIterator first, ForwardIterator last) -> flexible_immutable_array_builder
    {
        assert(std::distance(first, last) > 0);
        return flexible_immutable_array_builder(flexible_immutable_array<T>::move_from_range(first, last));
    }

} // namespace aeh::msp
