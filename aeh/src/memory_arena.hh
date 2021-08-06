#pragma once

#include "virtual_memory.hh"

namespace aeh
{

	struct MemoryArena
	{
		MemoryArena() noexcept = default;
		MemoryArena(MemoryArena const &) = delete;
		MemoryArena(MemoryArena &&) noexcept;
		auto operator = (MemoryArena const &) = delete;
		auto operator = (MemoryArena &&) noexcept -> MemoryArena &;

		[[nodiscard]] static auto with_virtual_region(size_t virtual_region_size_in_bytes) noexcept -> MemoryArena;

		[[nodiscard]] auto allocate(size_t bytes, size_t alignment) noexcept -> void *;
		[[nodiscard]] auto allocated_bytes() const noexcept -> size_t { return allocated_byte_count; }
		auto free_up_to(size_t number_of_bytes_to_keep) noexcept -> void;
		auto free_all() noexcept -> void { free_up_to(0); }

	private:
		VirtualMemoryRegion virtual_memory_region;
		size_t allocated_byte_count = 0;
	};

	struct MemoryArenaScopeGuard
	{
		[[nodiscard]] explicit MemoryArenaScopeGuard(MemoryArena & arena) noexcept
			: guarded_memory_arena(&arena)
			, original_allocated_bytes(arena.allocated_bytes())
		{}

		~MemoryArenaScopeGuard() { guarded_memory_arena->free_up_to(original_allocated_bytes); }

		MemoryArenaScopeGuard(MemoryArenaScopeGuard const &) = delete;
		MemoryArenaScopeGuard(MemoryArenaScopeGuard &&) = delete;
		auto operator = (MemoryArenaScopeGuard const &) -> MemoryArenaScopeGuard & = delete;
		auto operator = (MemoryArenaScopeGuard &&) -> MemoryArenaScopeGuard & = delete;

	private:
		MemoryArena * guarded_memory_arena;
		size_t original_allocated_bytes;
	};

} // namespace aeh
