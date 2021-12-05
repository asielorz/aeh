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
		// Invalidate last saved if last saved was undoed and then overriden by a new push.
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
		if (!is_current_head())
		{
			currently_active++;
			return true;
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
	void ChangeStack<T>::save() noexcept
	{
		last_saved = currently_active;
	}

	//**************************************************************************************************************

	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	EditorState<T, StateFunction>::EditorState(StateFunction state_function_, T initial_value, size_t initial_reserve)
		: document(std::move(initial_value), initial_reserve)
		, state_function(std::move(state_function_))
	{
		update_state();
	}

	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::current() const noexcept -> T const &
	{
		return document.current(); 
	}
	
	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::extra_state() const noexcept -> ExtraState const &
	{
		return state; 
	}

	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::size() const noexcept -> size_t
	{
		return document.size(); 
	}
	
	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::current_index() const noexcept -> size_t
	{
		return document.current_index(); 
	}
	
	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::is_current_head() const noexcept -> bool
	{
		return document.is_current_head(); 
	}
	
	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::has_unsaved_changes() const noexcept -> bool
	{
		return document.has_unsaved_changes(); 
	}

	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::push(T t) -> void
	{
		document.push(std::move(t));  
		update_state(); 
	}
	
	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::undo() noexcept -> bool
	{
		bool const changed = document.undo();
		if (changed)
			update_state();
		return changed; 
	}
	
	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::redo() noexcept -> bool
	{
		bool const changed = document.redo(); 
		if (changed)
			update_state(); 
		return changed; 
	}
	
	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::reset(T initial_value) noexcept -> void
	{
		document.reset(std::move(initial_value)); 
		update_state(); 
	}
	
	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::save() noexcept -> void
	{
		document.save(); 
	}

	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	auto EditorState<T, StateFunction>::update_state() -> void
	{
		state = std::invoke(std::as_const(state_function), current()); 
	}

} // namespace aeh
