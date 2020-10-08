namespace aeh
{

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	unique_handle<HandleT, deleter, null_value>::unique_handle(HandleT handle_) noexcept
		: handle(handle_)
	{}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	unique_handle<HandleT, deleter, null_value>::~unique_handle()
	{
		if (handle != null_value)
			deleter(handle);
	}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	unique_handle<HandleT, deleter, null_value>::unique_handle(unique_handle && other) noexcept
		: handle(other.release())
	{}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	auto unique_handle<HandleT, deleter, null_value>::operator = (unique_handle && other) noexcept -> unique_handle &
	{
		handle = other.release();
		return *this;
	}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	HandleT unique_handle<HandleT, deleter, null_value>::get() const noexcept
	{
		return handle;
	}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	HandleT unique_handle<HandleT, deleter, null_value>::release() noexcept
	{
		HandleT temp = handle;
		handle = null_value;
		return temp;
	}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	void unique_handle<HandleT, deleter, null_value>::reset(HandleT handle_) noexcept
	{
		HandleT temp = handle;
		handle = handle_;
		deleter(temp);
	}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	unique_handle<HandleT, deleter, null_value>::operator bool() const noexcept
	{
		return get() != null_value;
	}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	unique_handle<HandleT, deleter, null_value>::view::view() noexcept
		: handle(null_value)
	{}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	unique_handle<HandleT, deleter, null_value>::view::view(HandleT handle_) noexcept
		: handle(handle_)
	{}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	unique_handle<HandleT, deleter, null_value>::view::view(const unique_handle & handle_) noexcept
		: handle(handle_.get())
	{}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	unique_handle<HandleT, deleter, null_value>::view::operator HandleT() const noexcept
	{
		return handle;
	}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	HandleT unique_handle<HandleT, deleter, null_value>::view::get() const noexcept
	{
		return handle;
	}

	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value>
	unique_handle<HandleT, deleter, null_value>::view::operator bool() const noexcept
	{
		return get() != null_value;
	}

} // namespace aeh
