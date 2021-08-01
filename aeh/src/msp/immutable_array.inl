namespace aeh::msp
{

    namespace detail
    {
        extern shared<flexible_immutable_array<int>> & the_empty_array;

        template <typename T>
        flexible_immutable_array<T>::~flexible_immutable_array()
        {
            size_t const n = size_;
            T * elements = data_;
            std::destroy_n(std::reverse_iterator<T *>(elements + n), n - 1);
        }

        template <typename T>
        auto flexible_immutable_array<T>::make_empty() -> shared_ptr<flexible_immutable_array>
        {
            return shared_ptr<flexible_immutable_array>(reinterpret_cast<shared<flexible_immutable_array> *>(&detail::the_empty_array));
        }
    } // namespace detail

    template <typename T>
    immutable_array<T>::immutable_array() noexcept
        : ptr(detail::flexible_immutable_array<T>::make_empty())
    {}

    template <typename T>
    immutable_array<T>::immutable_array(immutable_array && other) noexcept
        : ptr(std::move(other.ptr))
    {
        other.ptr = detail::flexible_immutable_array<T>::make_empty();
    }

    template <typename T>
    auto immutable_array<T>::operator = (immutable_array && other) noexcept -> immutable_array &
    {
        ptr = std::move(other.ptr);
        other.ptr = detail::flexible_immutable_array<T>::make_empty();
        return *this;
    }

    template <typename T>
    auto immutable_array<T>::acquire_ownership_of(shared_ptr<detail::flexible_immutable_array<T>> && ptr_) noexcept -> immutable_array
    {
        immutable_array array;
        array.ptr = std::move(ptr_);
        return array;
    }

    template <typename T>
    auto immutable_array<T>::from_ilist(std::initializer_list<T> ilist) -> immutable_array
    {
        return copy_of_range(ilist.begin(), ilist.end());
    }

    template <typename T>
    template <std::forward_iterator ForwardIterator> requires std::convertible_to<std::iter_value_t<ForwardIterator>, T>
    auto immutable_array<T>::copy_of_range(ForwardIterator first, ForwardIterator last) -> immutable_array
    {
        size_t const size = std::distance(first, last);

        if (size == 0)
            return immutable_array();

        size_t const bytes_needed = sizeof(shared<detail::flexible_immutable_array<T>>) + sizeof(T) * (size - 1);
        auto new_array = static_cast<shared<detail::flexible_immutable_array<T>> *>(::operator new(bytes_needed));
        new_array->reference_counter = 0;
        new_array->value.size_ = size;
        std::uninitialized_copy(first, last, static_cast<T *>(new_array->value.data_));
        return acquire_ownership_of(shared_ptr<detail::flexible_immutable_array<T>>(new_array));
    }

    template <typename T>
    template <std::ranges::forward_range ForwardRange> requires std::convertible_to<std::ranges::range_value_t<ForwardRange>, T>
    auto immutable_array<T>::copy_of_range(ForwardRange const & range) -> immutable_array
    {
        return copy_of_range(range.begin(), range.end());
    }

    template <typename T>
    template <std::forward_iterator ForwardIterator> requires std::convertible_to<std::iter_value_t<ForwardIterator>, T>
    auto immutable_array<T>::move_from_range(ForwardIterator first, ForwardIterator last) -> immutable_array
    {
        size_t const size = std::distance(first, last);

        if (size == 0)
            return immutable_array();

        size_t const bytes_needed = sizeof(shared<detail::flexible_immutable_array<T>>) + sizeof(T) * (size - 1);
        auto new_array = static_cast<shared<detail::flexible_immutable_array<T>> *>(::operator new(bytes_needed));
        new_array->reference_counter = 0;
        new_array->value.size_ = size;
        std::uninitialized_move(first, last, static_cast<T *>(new_array->value.data_));
        return acquire_ownership_of(shared_ptr<detail::flexible_immutable_array<T>>(new_array));
    }

    template <typename T>
    template <std::ranges::forward_range ForwardRange> requires std::convertible_to<std::ranges::range_value_t<ForwardRange>, T>
    auto immutable_array<T>::move_from_range(ForwardRange && range) -> immutable_array
    {
        return move_from_range(range.begin(), range.end());
    }

    template <typename T>
    template <aeh::invocable_r<std::vector<T>, std::vector<T>> F>
    auto immutable_array<T>::transform(F && f) const -> immutable_array
    {
        std::vector<T> contents;
        size_t const n = size();
        contents.reserve(n * 2);
        contents.assign(begin(), end());
        auto transformed_contents = std::forward<F>(f)(std::move(contents));
        return move_from_range(transformed_contents.begin(), transformed_contents.end());
    }

    template <typename T>
    template <aeh::invocable_r<void, std::span<T>> F>
    auto immutable_array<T>::permute(F && f) const -> immutable_array
    {
        auto builder = immutable_array_builder<T>::copy_of_range(begin(), end());
        std::forward<F>(f)(std::span<T>(builder.data(), builder.size()));
        return builder.finish();
    }

    template <std::equality_comparable T>
    auto operator == (immutable_array<T> const & a, immutable_array<T> const & b) noexcept -> bool
    {
        return (a.data() == b.data()) || std::equal(a.begin(), a.end(), b.begin(), b.end());
    }

    template <typename T> 
    auto operator <=> (immutable_array<T> const & a, immutable_array<T> const & b) noexcept
    {
        return std::lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end());
    }

    template <typename T>
    auto immutable_array_builder<T>::with_size(size_t size) -> immutable_array_builder
    {
        assert(size > 0);
        size_t const bytes_needed = sizeof(shared<detail::flexible_immutable_array<T>>) + sizeof(T) * (size - 1);
        auto new_array = static_cast<shared<detail::flexible_immutable_array<T>> *>(::operator new(bytes_needed));
        new_array->reference_counter = 0;
        new_array->value.size_ = size;
        std::uninitialized_default_construct_n(static_cast<T *>(new_array->value.data_), size);
        return immutable_array_builder(immutable_array<T>::acquire_ownership_of(shared_ptr<detail::flexible_immutable_array<T>>(new_array)));
    }

    template <typename T>
    auto immutable_array_builder<T>::from_ilist(std::initializer_list<T> ilist) -> immutable_array_builder
    {
        assert(ilist.size() > 0);
        return immutable_array_builder(immutable_array<T>::from_ilist(ilist));
    }

    template <typename T>
    template <typename ForwardIterator>
    auto immutable_array_builder<T>::copy_of_range(ForwardIterator first, ForwardIterator last) -> immutable_array_builder
    {
        assert(std::distance(first, last) > 0);
        return immutable_array_builder(immutable_array<T>::copy_of_range(first, last));
    }

    template <typename T>
    template <typename ForwardIterator>
    auto immutable_array_builder<T>::move_from_range(ForwardIterator first, ForwardIterator last) -> immutable_array_builder
    {
        assert(std::distance(first, last) > 0);
        return immutable_array_builder(immutable_array<T>::move_from_range(first, last));
    }

} // namespace aeh::msp
