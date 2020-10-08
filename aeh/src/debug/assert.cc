#include "assert.hh"
#include <string>
#include <Windows.h>

namespace aeh::debug
{

	int message_box(const char * title, const char * message, unsigned flags) noexcept
	{
		return ::MessageBoxA(nullptr, message, title, flags);
	}

	int message_box_abort_retry_ignore(const char * title, const char * message) noexcept
	{
		const auto message_box_opt = message_box(title, message, MBType::AbortRetryIgnore | MBIcon::Error);

		switch (message_box_opt)
		{
		case MBRet::Abort:
			std::abort();
			break;
		case MBRet::Retry:
			__debugbreak();
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

	int not_implemented(char const location[])
	{
		if (location)
		{
			char buffer[2048];
			sprintf_s(buffer, "The functionality at %s has not been implemented.", location);
			return message_box_abort_retry_ignore("Not implemented", buffer);
		}
		else
		{
			return message_box_abort_retry_ignore("Not implemented", "The functionality has not been implemented.");
		}
	}

	namespace detail
	{
		void handle_debug_assert(bool expression, const char * expression_str, const char * msg, const char * file, const char * function, int line)
		{
			if (!(expression))
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
		}
	} // namespace detail

} // namespace aeh::debug
