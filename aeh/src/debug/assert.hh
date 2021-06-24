#pragma once

#include "compatibility.hh"

namespace aeh::debug
{
	enum struct MBType
	{
		AbortRetryIgnore = 0x00000002L,
		//CancelTryContinue = 0x00000006L,
		//Help = 0x00004000L,
		Ok = 0x00000000L,
		OkCancel = 0x00000001L,
		RetryCancel = 0x00000005L,
		YesNo = 0x00000004L,
		YesNoCancel = 0x00000003L
	};

	enum struct MBIcon
	{
		//Exclamation = 0x00000030L,
		Warning = 0x00000030L,
		Information = 0x00000040L,
		//Asterist = 0x00000040L,
		Question = 0x00000020L,
		//Stop = 0x00000010L,
		Error = 0x00000010L
	};
	struct MBFlags
	{
		MBType type;
		MBIcon icon;
	};
	constexpr MBFlags operator | (MBType type, MBIcon icon) noexcept { return MBFlags{ type, icon }; }

	enum struct MBRet
	{
		Ok = 1,
		Cancel = 2,
		Abort = 3,
		Retry = 4,
		Ignore = 5,
		Yes = 6,
		No = 7,
		//TryAgain = 10,
		//Continue = 11
	};

	MBRet message_box(const char * title, const char * message, MBFlags flags = MBType::Ok | MBIcon::Error) noexcept;

	//! Shows a message box with error icon and the abort, retry and ignore buttons.
	//! If abort is clicked, the program ends.
	//! If retry is clicked, the program breaks.
	//! If ignore is clicked, the program continues.
	//! Returns the clicked button.
	MBRet message_box_abort_retry_ignore(const char * title, const char * message) noexcept;

	//! Shows a message box with information icon and yes and no buttons. Returns true if yes is clicked.
	bool message_box_yes_no(const char* title, const char* message) noexcept;

	//! Shows a message box alerting that the called function has not been implemented. First shows an abort-retry-ignore message box, second uses an ok message box.
	MBRet not_implemented(char const location[] = nullptr);
	void not_implemented_ok(char const location[] = nullptr);

	//! Breaks into the debugger
	void debugbreak() noexcept;

	namespace detail
	{
		void debug_assert_failed(const char * expression_str, const char * msg, const char * file, const char * function, int line);
	}

} // namespace aeh::debug

#if AEH_DEBUG
	AEH_WARNING_PUSH()
	AEH_MSVC_PRAGMA(warning(disable : 4800)) // Forcing int/pointer to bool
	#define debug_assert_msg(expression,msg) if (!static_cast<bool>(expression)) { ::aeh::debug::detail::debug_assert_failed(#expression, msg, __FILE__, __FUNCTION__, __LINE__); }
	AEH_WARNING_POP()
#else
	#define debug_assert_msg(expression,msg) static_cast<void>(0);
#endif

#define debug_assert(expression) debug_assert_msg(expression, "debug_assert expression evaluated to false.")
