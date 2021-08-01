#pragma once

#include "compatibility.hh"

AEH_MSVC_WARNING_PUSH()
AEH_MSVC_WARNING_DISABLE(4355) // 'this' used as a base class initializer
AEH_MSVC_WARNING_DISABLE(4191) // conversion from different function pointer types
#	include <portable-file-dialogs.h>
AEH_MSVC_WARNING_POP()
