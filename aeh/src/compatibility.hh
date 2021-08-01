#pragma once

#define AEH_MINGW false

#ifdef __MINGW32__
#	undef AEH_MINGW
#	define AEH_MINGW true

#	define STRSAFE_NO_DEPRECATE // Don't remove sprintf and some other "unsafe" functions
#endif

#include <cstddef>
#include <cstdio>
#include <cstdlib>

#define AEH_STRINGIZE_IMPL(x) #x
#define AEH_STRINGIZE(x) AEH_STRINGIZE_IMPL(x)

#define AEH_INVOKE_UNARY(unary_function, argument) unary_function(argument)

#define AEH_EXPAND_IMPL(x) x
#define AEH_EXPAND(x) AEH_EXPAND_IMPL(x)

#define AEH_CONCAT_IMPL(a, b) a##b
#define AEH_CONCAT(a, b) AEH_CONCAT_IMPL(a, b)
#define AEH_UNIQUE_IDENTIFIER(prefix) AEH_CONCAT(prefix, __LINE__)

#define AEH_MESSAGE_LEVEL_DISABLED 0
#define AEH_MESSAGE_LEVEL_INFO 1
#define AEH_MESSAGE_LEVEL_DEBUG 2
#define AEH_MESSAGE_LEVEL_DIAG 3

#ifndef AEH_MESSAGE_LEVEL
#	define AEH_MESSAGE_LEVEL AEH_MESSAGE_LEVEL_INFO
#endif

#define AEH_MSVC false
#define AEH_GCC false
#define AEH_CLANG false

// clang seems to also define _MSC_VER and __GNUC__
#ifdef __clang__
#	undef AEH_CLANG
#	define AEH_CLANG true
#elif defined __GNUC__
#	undef AEH_GCC
#	define AEH_GCC true
#elif defined _MSC_VER
#	undef AEH_MSVC
#	define AEH_MSVC true
#endif

#if AEH_MSVC
#	define AEH_MSVC_PRAGMA(x) __pragma(x)
#	if AEH_MESSAGE_LEVEL >= AEH_MESSAGE_LEVEL_DIAG // Check manually because AEH_MESSAGE_LEVEL_DIAG is not yet defined
#		pragma message("MSVC detected: v" AEH_STRINGIZE(_MSC_VER))
#	endif
#else
#	define AEH_MSVC_PRAGMA(x)
#endif

#if AEH_GCC
#	define AEH_GCC_PRAGMA(x) _Pragma(#x)
#	if AEH_MESSAGE_LEVEL >= AEH_MESSAGE_LEVEL_DIAG // Check manually because AEH_MESSAGE_LEVEL_DIAG is not yet defined
#		pragma message("gcc detected: v" AEH_STRINGIZE(__GNUC__) "." AEH_STRINGIZE(__GNUC_MINOR__) "." AEH_STRINGIZE(__GNUC_PATCHLEVEL__))
#	endif
#else
#	define AEH_GCC_PRAGMA(x)
#endif

#if AEH_CLANG
#	define AEH_CLANG_PRAGMA(x) _Pragma(#x)
#	if AEH_MESSAGE_LEVEL >= AEH_MESSAGE_LEVEL_DIAG // Check manually because AEH_MESSAGE_LEVEL_DIAG is not yet defined
#		pragma message("clang detected: v" AEH_STRINGIZE(__clang_major__) "." AEH_STRINGIZE(__clang_minor__) "." AEH_STRINGIZE(__clang_patchlevel__))
#	endif
#else
#	define AEH_CLANG_PRAGMA(x)
#endif

#if AEH_MSVC
#	define AEH_PRAGMA_MESSAGE(x) __pragma(message(x))
#else
#	define AEH_PRAGMA_MESSAGE(x) _Pragma(AEH_STRINGIZE(message(x)))
#endif

#if AEH_MESSAGE_LEVEL >= AEH_MESSAGE_LEVEL_INFO
#	define AEH_MESSAGE_INFO(x) AEH_PRAGMA_MESSAGE(x)
#else
#	define AEH_MESSAGE_INFO(x)
#endif

#if AEH_MESSAGE_LEVEL >= AEH_MESSAGE_LEVEL_DEBUG
#	define AEH_MESSAGE_DEBUG(x) AEH_PRAGMA_MESSAGE(x)
#else
#	define AEH_MESSAGE_DEBUG(x)
#endif

#if AEH_MESSAGE_LEVEL >= AEH_MESSAGE_LEVEL_DIAG
#	define AEH_MESSAGE_DIAG(x) AEH_PRAGMA_MESSAGE(x)
#else
#	define AEH_MESSAGE_DIAG(x)
#endif

#define AEH_WARNING_PUSH() \
	AEH_MSVC_PRAGMA(warning(push))\
	AEH_GCC_PRAGMA(GCC diagnostic push)\
	AEH_CLANG_PRAGMA(clang diagnostic push)

#define AEH_WARNING_POP() \
	AEH_MSVC_PRAGMA(warning(pop))\
	AEH_GCC_PRAGMA(GCC diagnostic pop)\
	AEH_CLANG_PRAGMA(clang diagnostic pop)

#define AEH_IGNORE_WARNING_NAMELESS_STRUCT() \
	AEH_WARNING_PUSH()	\
	AEH_MSVC_PRAGMA(warning(disable : 4201)) /* nameless struct/union */ \
	AEH_GCC_PRAGMA(GCC diagnostic ignored "-Wpedantic") /* there is no more specific flag */ \
	AEH_CLANG_PRAGMA(clang diagnostic ignored "-Wgnu-anonymous-struct") \
	AEH_CLANG_PRAGMA(clang diagnostic ignored "-Wnested-anon-types")

#ifdef _WIN32
#	define AEH_WINDOWS true
#else
#	define AEH_WINDOWS false
#endif

#if defined(linux) || defined(__linux) || defined(__linux__) // From catch2
#	define AEH_LINUX true
#else
#	define AEH_LINUX false
#endif

// We use a macro so that MSVC's sprintf_s can deduce the size of 'buffer'
#if AEH_MSVC
#	define AEH_SPRINTF(buffer, format, ...) sprintf_s(buffer, format, __VA_ARGS__)
#else
#	define AEH_SPRINTF(buffer, format, ...) std::sprintf(buffer, format, __VA_ARGS__)
#endif

// We use a macro so that MSVC's sscanf_s can deduce the size of 'buffer'
#if AEH_MSVC
#	define AEH_SSCANF(buffer, format, ...) sscanf_s(buffer, format, __VA_ARGS__)
#else
#	define AEH_SSCANF(buffer, format, ...) std::sscanf(buffer, format, __VA_ARGS__)
#endif

#if AEH_MSVC
#	define AEH_FORCEINLINE __forceinline
#elif AEH_CLANG || AEH_GCC
#	define AEH_FORCEINLINE __attribute__((always_inline)) inline
#else
#	define AEH_FORCEINLINE inline
#endif
