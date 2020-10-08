namespace aeh
{

	template <typename T>
	ChangeStack<T>::ChangeStack(T initial_value, size_t initial_reserve)
	{
		changes.reserve(initial_reserve);
		changes.push_back(std::move(initial_value));
	}

	template <typename T>
	T const & ChangeStack<T>::current() const noexcept
	{
		return changes[currently_active];
	}

	template <typename T>
	size_t ChangeStack<T>::size() const noexcept
	{
		return changes.size();
	}

	template <typename T>
	size_t ChangeStack<T>::current_index() const noexcept
	{
		return currently_active;
	}

	template <typename T>
	bool ChangeStack<T>::is_current_head() const noexcept
	{
		return current_index() == size() - 1;
	}

	template <typename T>
	bool ChangeStack<T>::has_unsaved_changes() const noexcept
	{
		return last_saved != currently_active;
	}

	template <typename T>
	void ChangeStack<T>::push(T t)
	{
		changes.resize(++currently_active);
		if (last_saved > changes.size())
			last_saved = size_t(-1);
		changes.push_back(std::move(t));
	}

	template <typename T>
	bool ChangeStack<T>::undo() noexcept
	{
		if (currently_active > 0)
		{
			currently_active--;
			return true;
		}
		return false;
	}

	template <typename T>
	bool ChangeStack<T>::redo() noexcept
	{
		if (currently_active < changes.size() - 1)
		{
			currently_active++; return true;
		}
		return false;
	}

	template <typename T>
	void ChangeStack<T>::reset(T initial_value) noexcept
	{
		changes.clear();
		changes.push_back(std::move(initial_value));
		currently_active = 0;
		last_saved = 0;
	}

	template <typename T>
	void ChangeStack<T>::overwrite_current(T new_current) noexcept
	{
		changes[currently_active] = std::move(new_current);
	}

	template <typename T>
	void ChangeStack<T>::save() noexcept
	{
		last_saved = currently_active;
	}

} // namespace aeh
