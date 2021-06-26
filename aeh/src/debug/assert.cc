#include "assert.hh"
#include "unreachable.hh"
#include "internal/portable_file_dialogs.hh"
#include <string>

namespace aeh::debug
{
	namespace assert_locals
	{
		[[nodiscard]] static constexpr pfd::choice type_to_pfd(MBType type) noexcept
		{
			switch (type)
			{
			case aeh::debug::MBType::AbortRetryIgnore:
				return pfd::choice::abort_retry_ignore;
			case aeh::debug::MBType::Ok:
				return pfd::choice::ok;
			case aeh::debug::MBType::OkCancel:
				return pfd::choice::ok_cancel;
			case aeh::debug::MBType::RetryCancel:
				return pfd::choice::retry_cancel;
			case aeh::debug::MBType::YesNo:
				return pfd::choice::yes_no;
			case aeh::debug::MBType::YesNoCancel:
				return pfd::choice::yes_no_cancel;
			}

			declare_unreachable();
		}

		[[nodiscard]] static constexpr pfd::icon icon_to_pfd(MBIcon icon) noexcept
		{
			switch (icon)
			{
			case aeh::debug::MBIcon::Warning:
				return pfd::icon::warning;
			case aeh::debug::MBIcon::Information:
				return pfd::icon::info;
			case aeh::debug::MBIcon::Question:
				return pfd::icon::question;
			case aeh::debug::MBIcon::Error:
				return pfd::icon::error;
			}

			declare_unreachable();
		}

		[[nodiscard]] static constexpr MBRet pfd_button_to_mbret(pfd::button button) noexcept
		{
			switch (button)
			{
			case pfd::button::cancel:
				return MBRet::Cancel;
			case pfd::button::ok:
				return MBRet::Ok;
			case pfd::button::yes:
				return MBRet::Yes;
			case pfd::button::no:
				return MBRet::No;
			case pfd::button::abort:
				return MBRet::Abort;
			case pfd::button::retry:
				return MBRet::Retry;
			case pfd::button::ignore:
				return MBRet::Ignore;
			}

			declare_unreachable();
		}
	} // namespace assert_locals

	MBRet message_box(const char * title, const char * message, MBFlags flags) noexcept
	{
		const pfd::choice type = assert_locals::type_to_pfd(flags.type);
		const pfd::icon icon = assert_locals::icon_to_pfd(flags.icon);

		const pfd::button result_button = pfd::message(title, message, type, icon).result();

		return assert_locals::pfd_button_to_mbret(result_button);
	}

	MBRet message_box(const char8_t * title, const char8_t * message, MBFlags flags) noexcept
	{
		return message_box(reinterpret_cast<const char*>(title), reinterpret_cast<const char*>(message), flags);
	}

	MBRet message_box_abort_retry_ignore(const char * title, const char * message) noexcept
	{
		const auto message_box_opt = message_box(title, message, MBType::AbortRetryIgnore | MBIcon::Error);

		switch (message_box_opt)
		{
		case MBRet::Abort:
			std::abort();
			break;
		case MBRet::Retry:
			AEH_DEBUGBREAK();
			break;
		default:
			break;
		}

		return message_box_opt;
	}

	bool message_box_yes_no(const char * title, const char * message) noexcept
	{
		const auto message_box_opt = message_box(title, message, MBType::YesNo | MBIcon::Information);
		return message_box_opt == MBRet::Yes;
	}

	MBRet not_implemented(char const location[])
	{
		if (location)
		{
			char buffer[2048];
			AEH_SPRINTF(buffer, "The functionality at %s has not been implemented.", location);
			return message_box_abort_retry_ignore("Not implemented", buffer);
		}
		else
		{
			return message_box_abort_retry_ignore("Not implemented", "The functionality has not been implemented.");
		}
	}

	void not_implemented_ok(char const location[])
	{
		if (location)
		{
			char buffer[2048];
			AEH_SPRINTF(buffer, "The functionality at %s has not been implemented.", location);
			message_box("Not implemented", buffer);
		}
		else
		{
			message_box("Not implemented", "The functionality has not been implemented.");
		}
	}

	namespace detail
	{
		void debug_assert_failed(const char * expression_str, const char * msg, const char * file, const char * function, int line)
		{
			/* Remove the folders from the path because they bloat and don't add much information */
			::std::string file_name_ = file;
			file_name_ = file_name_.substr(file_name_.find_last_of('\\') + 1, std::string::npos);

			message_box_abort_retry_ignore(
				"Assertion failed",
				(std::string(msg) +
					"\nExpression: \"" + expression_str + "\"\n"
					"Function: \"" + function + "\"\n"
					"File: \"" + file_name_ + "\"\n"
					"Line: " + ::std::to_string(line)).c_str());
		}
	} // namespace detail

} // namespace aeh::debug
