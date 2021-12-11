#pragma once

#include "function_ptr.hh"

namespace aeh
{

	//! Abstraction for a handle with unique ownership over it. It's movable and will invoke the deleter
	//! on the destructor
	template <typename HandleT, function_ptr<void(HandleT)> deleter, HandleT null_value = HandleT(0)>
	struct unique_handle
	{
		using handle_type = HandleT;

		unique_handle() noexcept = default;
		explicit unique_handle(HandleT handle_) noexcept;
		~unique_handle();

		unique_handle(const unique_handle &) = delete;
		unique_handle & operator = (const unique_handle &) = delete;
		unique_handle(unique_handle &&) noexcept;
		unique_handle & operator = (unique_handle &&) noexcept;

		[[nodiscard]] HandleT get() const noexcept;
		[[nodiscard]] HandleT release() noexcept;
		void reset(HandleT handle_ = null_value) noexcept;

		explicit operator bool() const noexcept;

		//! Non owning view over a handle managed by a unique_handle object
		struct mutable_view
		{
			mutable_view() noexcept;
			explicit mutable_view(HandleT handle_) noexcept;
			mutable_view(unique_handle & handle_) noexcept;

			explicit operator HandleT() const noexcept;
			[[nodiscard]] HandleT get() const noexcept;

			explicit operator bool() const noexcept;

		private:
			HandleT handle;
		};

		struct view
		{
			view() noexcept;
			explicit view(HandleT handle_) noexcept;
			view(const unique_handle & handle_) noexcept;
			view(mutable_view handle_) noexcept;

			explicit operator HandleT() const noexcept;
			[[nodiscard]] HandleT get() const noexcept;

			explicit operator bool() const noexcept;

		private:
			HandleT handle;
		};

	private:
		HandleT handle = null_value;
	};

} // namespace aeh

#include "unique_handle.inl"
