namespace aeh
{

	template <typename T, size_t Capacity>
	constexpr fixed_capacity_vector<T, Capacity>::~fixed_capacity_vector() requires(!std::is_trivially_destructible_v<T>)
	{
		clear();
	}

	template <typename T, size_t Capacity>
	constexpr fixed_capacity_vector<T, Capacity>::fixed_capacity_vector(std::initializer_list<T> ilist) noexcept
		: size_(ilist.size())
	{
		debug_assert(ilist.size() <= capacity());
		std::uninitialized_copy(ilist.begin(), ilist.end(), data());
	}

	template <typename T, size_t Capacity>
	constexpr fixed_capacity_vector<T, Capacity>::fixed_capacity_vector(fixed_capacity_vector const & other) noexcept(std::is_nothrow_copy_constructible_v<T>) requires(!std::is_trivially_copy_constructible_v<T>)
		: size_(other.size())
	{
		std::uninitialized_copy_n(other.data(), other.size(), data());
	}

	template <typename T, size_t Capacity>
	constexpr fixed_capacity_vector<T, Capacity>::fixed_capacity_vector(fixed_capacity_vector && other) noexcept(std::is_nothrow_move_constructible_v<T>) requires(!std::is_trivially_move_constructible_v<T>)
		: size_(other.size())
	{
		std::uninitialized_move_n(other.data(), other.size(), data());
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::operator = (fixed_capacity_vector const & other) noexcept(std::is_nothrow_copy_constructible_v<T>) -> fixed_capacity_vector & requires(!std::is_trivially_copy_constructible_v<T>)
	{
		clear();
		size_ = other.size();
		std::uninitialized_copy_n(other.data(), other.size(), data());
		return *this;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::operator = (fixed_capacity_vector && other) noexcept(std::is_nothrow_move_constructible_v<T>) -> fixed_capacity_vector & requires(!std::is_trivially_move_constructible_v<T>)
	{
		clear();
		size_ = other.size();
		std::uninitialized_move_n(other.data(), other.size(), data());
		return *this;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::size() const noexcept -> size_t
	{
		return size_;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::ssize() const noexcept -> ptrdiff_t
	{
		return ptrdiff_t(size());
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::int_size() const noexcept -> int requires(Capacity <= std::numeric_limits<int>::max())
	{
		return int(size());
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::empty() const noexcept -> bool
	{
		return size() == 0;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::capacity() noexcept -> size_t
	{
		return Capacity;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::max_size() noexcept -> size_t
	{
		return Capacity;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::data() noexcept -> T *
	{
		return std::launder(reinterpret_cast<T *>(&buffer));
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::data() const noexcept -> T const *
	{
		return std::launder(reinterpret_cast<T const *>(&buffer));
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::operator [] (size_t i) noexcept -> T &
	{
		debug_assert(i < size());
		return data()[i];
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::operator [] (size_t i) const noexcept -> T const &
	{
		debug_assert(i < size());
		return data()[i];
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::front() noexcept -> T &
	{
		return (*this)[0];
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::front() const noexcept -> T const &
	{
		return (*this)[0];
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::back() noexcept -> T &
	{
		return (*this)[size() - 1];
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::back() const noexcept -> T const &
	{
		return (*this)[size() - 1];
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::begin() noexcept -> iterator
	{
		return data();
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::end() noexcept -> iterator
	{
		return data() + size();
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::begin() const noexcept -> const_iterator
	{
		return cbegin();
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::end() const noexcept -> const_iterator
	{
		return cend();
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::cbegin() const noexcept -> const_iterator
	{
		return data();
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::cend() const noexcept -> const_iterator
	{
		return data() + size();
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::rbegin() noexcept -> reverse_iterator
	{
		return reverse_iterator(end());
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::rend() noexcept -> reverse_iterator
	{
		return reverse_iterator(begin());
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::rbegin() const noexcept -> const_reverse_iterator
	{
		return crbegin();
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::rend() const noexcept -> const_reverse_iterator
	{
		return crend();
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::crbegin() const noexcept -> const_reverse_iterator
	{
		return const_reverse_iterator(cend());
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::crend() const noexcept -> const_reverse_iterator
	{
		return const_reverse_iterator(cbegin());
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::nth(size_t i) noexcept -> iterator
	{
		debug_assert(i <= size());
		return begin() + i;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::nth(size_t i) const noexcept -> const_iterator
	{
		debug_assert(i <= size());
		return begin() + i;
	}

	template <typename T, size_t Capacity>
	constexpr fixed_capacity_vector<T, Capacity>::operator std::span<T>() noexcept
	{
		return std::span<T>(data(), size());
	}

	template <typename T, size_t Capacity>
	constexpr fixed_capacity_vector<T, Capacity>::operator std::span<T const>() const noexcept
	{
		return std::span<T const>(data(), size());
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::clear() noexcept -> void
	{
		std::destroy(begin(), end());
		size_ = 0;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::push_back(T const & t) noexcept(std::is_nothrow_copy_constructible_v<T>) -> T &
	{
		return emplace_back(t);
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::push_back(T && t) noexcept(std::is_nothrow_move_constructible_v<T>) -> T &
	{
		return emplace_back(std::move(t));
	}

	template <typename T, size_t Capacity>
	template <typename ... Args>
	constexpr auto fixed_capacity_vector<T, Capacity>::emplace_back(Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) -> T &
	{
		debug_assert(size() < capacity());
		T * const new_element = std::construct_at(data() + size(), std::forward<Args>(args)...);
		size_++;
		return *new_element;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::pop_back() noexcept -> void
	{
		std::destroy_at(&back());
		size_--;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::resize(size_t new_size) noexcept -> void
	{
		debug_assert(new_size <= capacity());
		if (new_size < size())
			std::destroy(begin() + new_size, end());
		else
			std::uninitialized_default_construct(begin() + size(), begin() + new_size);

		size_ = new_size;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::resize(size_t new_size, T const & value) noexcept -> void
	{
		debug_assert(new_size <= capacity());
		if (new_size < size())
			std::destroy(begin() + new_size, end());
		else
			std::uninitialized_fill(begin() + size(), begin() + new_size, value);

		size_ = new_size;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::erase(const_iterator pos) noexcept -> iterator
	{
		debug_assert(pos >= begin() && pos < end());
		iterator const pos_mutable = nth(pos - begin());
		std::rotate(pos_mutable, pos_mutable + 1, end());
		std::destroy_at(&back());
		size_--;
		return pos_mutable;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::erase(const_iterator first, const_iterator last) noexcept -> const_iterator
	{
		ptrdiff_t const n = std::distance(first, last);
		if (std::distance(first, last) <= 0)
			return nth(first - begin());

		debug_assert(first < last);
		debug_assert(first >= begin() && first < end());
		debug_assert(last > begin() && last <= end());
		iterator const first_mutable = nth(first - begin());
		iterator const first_to_destroy = std::rotate(first_mutable, first_mutable + n, end());
		std::destroy_n(first_to_destroy, n);
		size_ -= n;
		return first_mutable;
	}

	template <typename T, size_t Capacity>
	template <std::ranges::input_range R>
	constexpr auto fixed_capacity_vector<T, Capacity>::assign(R const & range)
		noexcept(std::is_nothrow_constructible_v<T, std::ranges::range_value_t<R>>)
		-> void
		requires(std::is_constructible_v<T, std::ranges::range_value_t<R>>)
	{
		assign(range.begin(), range.end());
	}

	template <typename T, size_t Capacity>
	template <std::input_iterator It, std::sentinel_for<It> Sentinel>
	constexpr auto fixed_capacity_vector<T, Capacity>::assign(It first, Sentinel last)
		noexcept(std::is_nothrow_constructible_v<T, std::iter_value_t<It>>)
		-> void
		requires(std::is_constructible_v<T, std::iter_value_t<It>>)
	{
		debug_assert(std::distance(first, last) <= Capacity);
		clear();
		for (It it = first; it != last; ++it)
			emplace_back(*it);
	}

	template <typename T, size_t Capacity>
	constexpr auto operator == (fixed_capacity_vector<T, Capacity> const & a, fixed_capacity_vector<T, Capacity> const & b) noexcept -> bool
	{
		return std::equal(a.begin(), a.end(), b.begin(), b.end());
	}

	template <typename T, size_t Capacity>
	constexpr auto operator <=> (fixed_capacity_vector<T, Capacity> const & a, fixed_capacity_vector<T, Capacity> const & b) noexcept
	{
		return std::lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end());
	}

} // namespace aeh
