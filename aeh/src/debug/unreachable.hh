#pragma once

namespace aeh::debug
{
	[[noreturn]] auto declare_unreachable() noexcept -> void;
	[[noreturn]] auto mark_as_to_do() noexcept -> void;
	[[noreturn]] auto mark_as_to_do(char const what[]) noexcept -> void;
} // namespace aeh::debug
