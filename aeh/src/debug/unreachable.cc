#include "unreachable.hh"
#include "assert.hh"
#include <cstdlib>
#include <cstdio>

namespace aeh::debug
{

	namespace unreachable_locals
	{
		[[noreturn]] static void handle_unreachable(char const * title, char const * message) noexcept
		{
			MBRet const ret = message_box(title, message, MBType::RetryCancel | MBIcon::Error);
			if (ret == MBRet::Retry)
				debugbreak();

			std::quick_exit(1);
		}
	}

	[[noreturn]] auto declare_unreachable() noexcept -> void
	{
		unreachable_locals::handle_unreachable("Unreachable code", "Control flow is not allowed to reach this point");
	}

	[[noreturn]] auto mark_as_to_do() noexcept -> void
	{
		unreachable_locals::handle_unreachable("TODO", "This part of the code has not been implemented yet.");
	}

	[[noreturn]] auto mark_as_to_do(char const what[]) noexcept -> void
	{
		char buffer[1024];
		AEH_SPRINTF(buffer, "This part of the code (%s) has not been implemented yet.", what);

		unreachable_locals::handle_unreachable("TODO", buffer);
	}

} // namespace aeh::debug
