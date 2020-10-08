#pragma once

#include "compatibility.hh"

#define AEH_WARNING_HEADER(warning_code)			\
	__FILE__ "(" AEH_INVOKE_UNARY(AEH_STRINGIZE, __LINE__) ") : Warning " warning_code " : "


#define WARNING(WarningCode, WarningText) AEH_MSVC_PRAGMA(message(AEH_WARNING_HEADER(WarningCode) WarningText))

#define TODO(WarningText) WARNING("TODO", WarningText)
#define HACK(WarningText) WARNING("HACK", WarningText)
