#pragma once

#include <vector>

namespace aeh
{

	template <typename T>
	struct ChangeStack
	{
		explicit ChangeStack(T initial_value = T(), size_t initial_reserve = 32);

		T const & current() const noexcept;
		size_t size() const noexcept;
		size_t current_index() const noexcept;
		bool is_current_head() const noexcept;
		bool has_unsaved_changes() const noexcept;

		void push(T t);
		bool undo() noexcept;
		bool redo() noexcept;
		void reset(T initial_value) noexcept;
		void overwrite_current(T new_current) noexcept;
		void save() noexcept;

	private:
		std::vector<T> changes;
		size_t currently_active = 0;
		size_t last_saved = 0;
	};

} // namespace aeh

#include "change_stack.inl"
