namespace aeh
{

	template <std::move_constructible T, typename Allocator>
	constexpr ring<T, Allocator>::ring(Allocator allocator) noexcept
		: allocator_(std::move(allocator))
	{}

	template <std::move_constructible T, typename Allocator>
	constexpr ring<T, Allocator>::~ring()
	{
		reset();
	}

	template <std::move_constructible T, typename Allocator>
	constexpr ring<T, Allocator>::ring(const ring& other) requires std::copy_constructible<T>
	{
		reserve(other.size());
		const auto [r1, r2] = other.data();
		for (const T& t : r1)
			push_back(t);
		for (const T& t : r2)
			push_back(t);
	}

	template <std::move_constructible T, typename Allocator>
	constexpr ring<T, Allocator>::ring(ring&& other) noexcept
		: buffer_(other.buffer_)
		, capacity_(other.capacity_)
		, first_(other.first_)
		, size_(other.size_)
	{
		other.buffer_ = nullptr;
		other.capacity_ = 0;
		other.first_ = 0;
		other.size_ = 0;
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::operator = (const ring& other) -> ring& requires std::copy_constructible<T>
	{
		clear();
		reserve(other.size());
		const auto [r1, r2] = other.data();
		for (const T& t : r1)
			push_back(t);
		for (const T& t : r2)
			push_back(t);
		return *this;
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::operator = (ring&& other) noexcept -> ring&
	{
		reset();
		buffer_ = other.buffer_;
		capacity_ = other.capacity_;
		first_ = other.first_;
		size_ = other.size_;
		other.buffer_ = nullptr;
		other.capacity_ = 0;
		other.first_ = 0;
		other.size_ = 0;
		return *this;
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::empty() const noexcept -> bool
	{
		return size() == 0;
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::size() const noexcept -> size_t
	{
		return size_;
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::capacity() const noexcept -> size_t
	{
		return capacity_;
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::front() noexcept -> T&
	{
		assert(!empty());
		return *(buffer_ + first_);
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::front() const noexcept -> const T&
	{
		assert(!empty());
		return *(buffer_ + first_);
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::back() noexcept -> T&
	{
		assert(!empty());
		size_t const last = (first_ + size_ - 1) % capacity_;
		return *(buffer_ + last);
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::back() const noexcept -> const T&
	{
		assert(!empty());
		size_t const last = (first_ + size_ - 1) % capacity_;
		return *(buffer_ + last);
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::push_back(const T& value) -> T&
		requires std::copy_constructible<T>
	{
		return emplace_back(value);
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::push_back(T&& value) -> T&
	{
		return emplace_back(std::move(value));
	}

	template <std::move_constructible T, typename Allocator>
	template <typename ... Args> requires std::constructible_from<T, Args...>
	constexpr auto ring<T, Allocator>::emplace_back([[maybe_unused]] Args&& ... args) -> T&
	{
		if (size() == capacity())
		{
			change_capacity_to(size() == 0 ? 16 : size() * 3 / 2);
		}

		size_t const insertion_point = (first_ + size_) % capacity_;
		T* const new_element = buffer_ + insertion_point;

		std::uninitialized_construct_using_allocator(new_element, allocator_, std::forward<Args>(args)...);
		size_ += 1;
		return *new_element;
	}

	template <std::move_constructible T, typename Allocator>
	constexpr void ring<T, Allocator>::pop_front() noexcept
	{
		assert(!empty());

		(buffer_ + first_)->~T();
		first_ = (first_ + 1) % capacity_;
		size_ -= 1;
	}

	template <std::move_constructible T, typename Allocator>
	constexpr void ring<T, Allocator>::clear() noexcept
	{
		if (!empty())
		{
			auto const [r1, r2] = data();
			for (T& t : r1) std::allocator_traits<Allocator>::destroy(allocator_, &t);
			for (T& t : r2) std::allocator_traits<Allocator>::destroy(allocator_, &t);
			first_ = 0;
			size_ = 0;
		}
	}

	template <std::move_constructible T, typename Allocator>
	constexpr void ring<T, Allocator>::reset() noexcept
	{
		clear();
		std::allocator_traits<Allocator>::deallocate(allocator_, buffer_, capacity_);
		buffer_ = nullptr;
		capacity_ = 0;
	}

	template <std::move_constructible T, typename Allocator>
	constexpr void ring<T, Allocator>::reserve(size_t new_capacity)
	{
		if (capacity() < new_capacity)
			change_capacity_to(new_capacity);
	}

	template <std::move_constructible T, typename Allocator>
	constexpr void ring<T, Allocator>::shrink_to_fit()
	{
		change_capacity_to(size());
	}

	template <std::move_constructible T, typename Allocator>
	constexpr auto ring<T, Allocator>::get_allocator() const noexcept -> Allocator
	{
		return allocator_;
	}

	template <std::move_constructible T, typename Allocator>
	constexpr void ring<T, Allocator>::change_capacity_to(size_t new_capacity)
	{
		if (new_capacity == capacity())
			return;

		T* const new_buffer = (new_capacity == 0) ? nullptr : std::allocator_traits<Allocator>::allocate(allocator_, new_capacity);
		size_t const new_size = std::min(size_, new_capacity);

		// Move and release the old buffer
		if (capacity_ > 0)
		{
			auto const [r1, r2] = data();

			size_t const first_move = std::min(r1.size(), new_size);
			std::uninitialized_move_n(r1.data(), first_move, new_buffer);

			size_t const second_move = std::min(r2.size(), new_size - first_move);
			std::uninitialized_move_n(r2.data(), second_move, new_buffer + first_move);
			
			for (T& t : r1) std::allocator_traits<Allocator>::destroy(allocator_, &t);
			for (T& t : r2) std::allocator_traits<Allocator>::destroy(allocator_, &t);
			std::allocator_traits<Allocator>::deallocate(allocator_, buffer_, capacity_);
		}

		buffer_ = new_buffer;
		capacity_ = new_capacity;
		first_ = 0;
		size_ = new_size;
	}

	template <std::move_constructible T, typename Allocator>
	constexpr std::pair<std::span<T>, std::span<T>> ring<T, Allocator>::data() noexcept
	{
		size_t last = first_ + size_;
		size_t first_range_size;
		size_t second_range_size;
		if (last < capacity_)
		{
			first_range_size = size_;
			second_range_size = 0;
		}
		else
		{
			first_range_size = capacity_ - first_;
			second_range_size = last - capacity_;
		}

		return {
			std::span<T>(buffer_ + first_, first_range_size),
			std::span<T>(buffer_, second_range_size),
		};
	}

	template <std::move_constructible T, typename Allocator>
	constexpr std::pair<std::span<T const>, std::span<T const>> ring<T, Allocator>::data() const noexcept
	{
		size_t last = first_ + size_;
		size_t first_range_size;
		size_t second_range_size;
		if (last < capacity_)
		{
			first_range_size = size_;
			second_range_size = 0;
		}
		else
		{
			first_range_size = capacity_ - first_;
			second_range_size = last - capacity_;
		}

		return {
			std::span<T const>(buffer_ + first_, first_range_size),
			std::span<T const>(buffer_, second_range_size),
		};
	}

}
