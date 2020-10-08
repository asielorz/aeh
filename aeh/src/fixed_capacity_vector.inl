namespace aeh
{

	template <typename Self>
	fixed_capacity_vector_crtp_base<Self>::~fixed_capacity_vector_crtp_base()
	{
		static_cast<Self *>(this)->clear();
	}

	template <typename T, size_t Capacity>
	constexpr fixed_capacity_vector<T, Capacity>::fixed_capacity_vector(std::initializer_list<T> ilist) noexcept
		: size_(ilist.size())
	{
		debug_assert(ilist.size() < capacity());
		std::uninitialized_copy(ilist.begin(), ilist.end(), data());
	}

	template <typename T, size_t Capacity>
	constexpr fixed_capacity_vector<T, Capacity>::fixed_capacity_vector(fixed_capacity_vector const & other) noexcept(std::is_nothrow_copy_constructible_v<T>)
		: size_(other.size())
	{
		std::uninitialized_copy_n(other.data(), other.size(), data());
	}

	template <typename T, size_t Capacity>
	constexpr fixed_capacity_vector<T, Capacity>::fixed_capacity_vector(fixed_capacity_vector && other) noexcept(std::is_nothrow_move_constructible_v<T>)
		: size_(other.size())
	{
		std::uninitialized_move_n(other.data(), other.size(), data());
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::operator = (fixed_capacity_vector const & other) noexcept(std::is_nothrow_copy_constructible_v<T>) -> fixed_capacity_vector &
	{
		clear();
		size_ = other.size();
		std::uninitialized_copy_n(other.data(), other.size(), data());
		return *this;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::operator = (fixed_capacity_vector && other) noexcept(std::is_nothrow_move_constructible_v<T>) -> fixed_capacity_vector &
	{
		clear();
		size_ = other.size();
		std::uninitialized_copy_n(other.data(), other.size(), data());
		return *this;
	}

	template <typename T, size_t Capacity>
	constexpr auto fixed_capacity_vector<T, Capacity>::size() const noexcept -> size_t
	{
		return size_;
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
	constexpr fixed_capacity_vector<T, Capacity>::operator span<T>() const noexcept
	{
		return { data(), size() };
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
		T * const new_element = ::new(data() + size()) T(std::forward<Args>(args)...);
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
	constexpr auto operator == (fixed_capacity_vector<T, Capacity> const & a, fixed_capacity_vector<T, Capacity> const & b) noexcept -> bool
	{
		return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), b.end());
	}

	template <typename T, size_t Capacity>
	constexpr auto operator != (fixed_capacity_vector<T, Capacity> const & a, fixed_capacity_vector<T, Capacity> const & b) noexcept -> bool
	{
		return !(a == b);
	}

	template <typename T, size_t Capacity>
	constexpr auto operator < (fixed_capacity_vector<T, Capacity> const & a, fixed_capacity_vector<T, Capacity> const & b) noexcept -> bool
	{
		return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), std::less());
	}

	template <typename T, size_t Capacity>
	constexpr auto operator <= (fixed_capacity_vector<T, Capacity> const & a, fixed_capacity_vector<T, Capacity> const & b) noexcept -> bool
	{
		return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), std::less_equal());
	}

	template <typename T, size_t Capacity>
	constexpr auto operator > (fixed_capacity_vector<T, Capacity> const & a, fixed_capacity_vector<T, Capacity> const & b) noexcept -> bool
	{
		return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), std::greater());
	}

	template <typename T, size_t Capacity>
	constexpr auto operator >= (fixed_capacity_vector<T, Capacity> const & a, fixed_capacity_vector<T, Capacity> const & b) noexcept -> bool
	{
		return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), std::greater_equal());
	}

} // namespace aeh
