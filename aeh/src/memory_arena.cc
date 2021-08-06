#include "memory_arena.hh"
#include "align.hh"
#include <algorithm>

namespace aeh
{

	MemoryArena::MemoryArena(MemoryArena && other) noexcept
		: virtual_memory_region(std::move(other.virtual_memory_region))
		, allocated_byte_count(other.allocated_byte_count)
	{
		other.allocated_byte_count = 0;
	}

	auto MemoryArena::operator = (MemoryArena && other) noexcept -> MemoryArena &
	{
		virtual_memory_region = std::move(other.virtual_memory_region);
		allocated_byte_count = other.allocated_byte_count;
		other.allocated_byte_count = 0;
		return *this;
	}

	auto MemoryArena::with_virtual_region(size_t virtual_memory_region_size_in_bytes) noexcept -> MemoryArena
	{
		MemoryArena new_arena;
		new_arena.virtual_memory_region = VirtualMemoryRegion::reserve_bytes_of_virtual_memory(virtual_memory_region_size_in_bytes);

		// Allocate a page of memory from the beginning.
		new_arena.virtual_memory_region.allocate_extra_pages_of_physical_memory(1);

		return new_arena;
	}

	auto MemoryArena::allocate(size_t bytes, size_t alignment) noexcept -> void *
	{
		size_t const aligned_start = align(allocated_byte_count, alignment);
		size_t const allocated_end = aligned_start + bytes;

		// Grow exponentially, except if the requested allocation requires growing even more.
		if (allocated_end > virtual_memory_region.physical_memory_size_in_bytes())
		{
			size_t const memory_pages_needed_to_hold_current_allocation = align_to_os_memory_page_boundary(allocated_end) / os_memory_page_size;
			size_t const twice_the_current_pages = virtual_memory_region.physical_memory_size_in_pages() * 2;
			size_t const new_pages_to_allocate = std::max(memory_pages_needed_to_hold_current_allocation, twice_the_current_pages) - virtual_memory_region.physical_memory_size_in_pages();
			virtual_memory_region.allocate_extra_pages_of_physical_memory(new_pages_to_allocate);
		}

		allocated_byte_count = allocated_end;
		return static_cast<char *>(virtual_memory_region.memory_region_address()) + aligned_start;
	}

	auto MemoryArena::free_up_to(size_t number_of_bytes_to_keep) noexcept -> void
	{
		allocated_byte_count = number_of_bytes_to_keep;

		// Shring by halving the amount of space in use, when even after halving there's still a whole page left.
		// This is aimed to avoid the pathological case of constantly allocating and deallocating when allocated size
		// is very close to reallocation boundary.
		if ((number_of_bytes_to_keep + os_memory_page_size) < virtual_memory_region.physical_memory_size_in_bytes() / 2)
			virtual_memory_region.free_all_pages_of_physical_memory_up_to(virtual_memory_region.physical_memory_size_in_pages() / 2);
	}

} // namespace aeh
