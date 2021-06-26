#pragma once

#include "compatibility.hh"

AEH_WARNING_PUSH()
AEH_MSVC_PRAGMA(warning(disable: 4355)) // 'this' used as a base class initializer
AEH_MSVC_PRAGMA(warning(disable: 4191)) // conversion from different function pointer types
#	include <portable-file-dialogs.h>
AEH_WARNING_POP()
