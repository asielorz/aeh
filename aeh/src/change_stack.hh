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
		void save() noexcept;

	private:
		std::vector<T> changes;
		size_t currently_active = 0;
		size_t last_saved = 0;
	};

	template <std::move_constructible T, std::regular_invocable<T const &> StateFunction>
	struct EditorState
	{
		using ExtraState = std::invoke_result_t<StateFunction const &, T const &>;

		explicit EditorState(StateFunction state_function_, T initial_value = T(), size_t initial_reserve = 32);

		auto current() const noexcept -> T const &;
		auto extra_state() const noexcept -> ExtraState const &;
		auto size() const noexcept -> size_t;
		auto current_index() const noexcept -> size_t;
		auto is_current_head() const noexcept -> bool;
		auto has_unsaved_changes() const noexcept -> bool;

		auto push(T t) -> void;
		auto undo() noexcept -> bool;
		auto redo() noexcept -> bool;
		auto reset(T initial_value) noexcept -> void;
		auto save() noexcept -> void;

	private:
		auto update_state() -> void;

		aeh::ChangeStack<T> document;
		ExtraState state;
		[[no_unique_address]] StateFunction state_function;
	};

} // namespace aeh

#include "change_stack.inl"
