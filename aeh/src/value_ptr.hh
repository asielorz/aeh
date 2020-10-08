#pragma once

#include "compatibility.hh"
#include <memory>

namespace aeh
{

	// unique_ptr but it's copyable and does deep copy.
	template <typename T>
	struct value_ptr : public std::unique_ptr<T>
	{
		static_assert(!std::is_array_v<T>, "T can't be an array type because we cannot copy it. If the array is of a size known in compile-time, you can use std::array.");
		static_assert(!std::is_polymorphic_v<T>, "T can't be polymorphic because value_ptr is supposed to be able to copy it.Use unique_ptr.");

		using std::unique_ptr<T>::unique_ptr;
		value_ptr() noexcept = default;
		value_ptr(value_ptr &&) noexcept = default;
		value_ptr & operator = (value_ptr &&) noexcept = default;

		value_ptr(std::unique_ptr<T> && ptr) noexcept : std::unique_ptr<T>(std::move(ptr)) {}

		value_ptr(value_ptr const & other)
			: std::unique_ptr<T>() // Silence gcc warning 'base class should be explicitly initialized in the copy constructor'
		{
			if (other)
				this->reset(new T(*other));
		}

		value_ptr & operator = (value_ptr const & other)
		{
			if (other)
				this->reset(new T(*other));
			else
				this->reset(nullptr);

			return *this;
		}

		template <typename T>
		static auto allocate(T t) -> value_ptr<T>
		{
			return std::make_unique<T>(std::move(t));
		}
	};

} // namespace aeh
