#include "virtual_memory.hh"
#include "compatibility.hh"
#include "align.hh"
#include "debug/assert.hh"

#if AEH_WINDOWS
	#include <Windows.h>
#elif AEH_LINUX
	#include <unistd.h>
	#include <sys/mman.h>
#endif

namespace aeh
{

#if AEH_WINDOWS

	size_t const os_memory_page_size = []
	{
		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		return static_cast<size_t>(system_info.dwPageSize);
	}();

#elif AEH_LINUX

	size_t const os_memory_page_size = static_cast<size_t>(getpagesize());

#endif

	auto align_to_os_memory_page_boundary(size_t n) noexcept -> size_t
	{
		return align(n, os_memory_page_size);
	}

	VirtualMemoryRegion::VirtualMemoryRegion(VirtualMemoryRegion && other) noexcept
		: memory(other.memory)
		, physical_memory_backed_size(other.physical_memory_backed_size)
		, virtual_region_size(other.virtual_region_size)
	{
		other.memory = nullptr;
		other.physical_memory_backed_size = 0;
		other.virtual_region_size = 0;
	}

	auto VirtualMemoryRegion::operator = (VirtualMemoryRegion && other) noexcept -> VirtualMemoryRegion &
	{
		free_virtual_memory_region();

		memory = other.memory;
		physical_memory_backed_size = other.physical_memory_backed_size;
		virtual_region_size = other.virtual_region_size;

		other.memory = nullptr;
		other.physical_memory_backed_size = 0;
		other.virtual_region_size = 0;

		return *this;
	}

	VirtualMemoryRegion::~VirtualMemoryRegion()
	{
		free_virtual_memory_region();
	}

	auto VirtualMemoryRegion::reserve_bytes_of_virtual_memory(size_t number_of_bytes) noexcept -> VirtualMemoryRegion
	{
		if (number_of_bytes == 0)
			return VirtualMemoryRegion();

		number_of_bytes = align_to_os_memory_page_boundary(number_of_bytes);

		VirtualMemoryRegion virtual_memory_region;

		#if AEH_WINDOWS
			virtual_memory_region.memory = VirtualAlloc(nullptr, number_of_bytes, MEM_RESERVE, PAGE_READWRITE);
		#elif AEH_LINUX
			virtual_memory_region.memory = mmap(nullptr, number_of_bytes, PROT_NONE, MAP_ANONYMOUS, -1, 0);
		#endif
		
		virtual_memory_region.virtual_region_size = number_of_bytes;

		return virtual_memory_region;
	}

	auto VirtualMemoryRegion::reserve_pages_of_virtual_memory(size_t number_of_pages) noexcept -> VirtualMemoryRegion
	{
		return reserve_bytes_of_virtual_memory(number_of_pages * os_memory_page_size);
	}

	auto VirtualMemoryRegion::allocate_extra_pages_of_physical_memory(size_t number_of_pages) noexcept -> bool
	{
		// Must have a region of virtual memory in order to be able to allocate physical memory.
		debug_assert(memory != nullptr);

		size_t const new_size_in_bytes = physical_memory_backed_size + number_of_pages * os_memory_page_size;
		debug_assert(new_size_in_bytes <= virtual_region_size);

		#if AEH_WINDOWS
			void const * const result = VirtualAlloc(memory, new_size_in_bytes, MEM_COMMIT, PAGE_READWRITE);
			if (result == nullptr)
				return false;
		#elif AEH_LINUX
			int const result = mprotect(memory, new_size_in_bytes, PROT_READ | PROT_WRITE);
			if (result != 0)
				return false;
		#endif
		physical_memory_backed_size = new_size_in_bytes;

		return true;
	}

	auto VirtualMemoryRegion::free_pages_of_physical_memory(size_t number_of_pages) noexcept -> bool
	{
		size_t const physical_memory_pages = physical_memory_size_in_pages();
		if (number_of_pages >= physical_memory_pages)
			return free_all_pages_of_physical_memory_up_to(0);
		else
			return free_all_pages_of_physical_memory_up_to(physical_memory_pages - number_of_pages);
	}

	auto VirtualMemoryRegion::free_all_pages_of_physical_memory_up_to(size_t number_of_pages_to_keep) noexcept -> bool
	{
		// Must have a region of virtual memory in order to be able to free physical memory.
		debug_assert(memory != nullptr);

		if (number_of_pages_to_keep < physical_memory_size_in_pages())
		{
			size_t const bytes_to_keep = number_of_pages_to_keep * os_memory_page_size;
			size_t const bytes_to_free = physical_memory_backed_size - bytes_to_keep;

			#if AEH_WINDOWS
				BOOL const free_ok = VirtualFree(static_cast<char *>(memory) + bytes_to_keep, bytes_to_free, MEM_DECOMMIT);
				if (!free_ok)
					return false;
			#elif AEH_LINUX
				int const result = mprotect(static_cast<char *>(memory) + bytes_to_keep, bytes_to_free, PROT_NONE);
				if (result != 0)
					return false;
			#endif

			physical_memory_backed_size = bytes_to_keep;
		}

		return true;
	}

	auto VirtualMemoryRegion::free_virtual_memory_region() noexcept -> bool
	{
		if (memory != nullptr)
		{
			#if AEH_WINDOWS
				BOOL const free_ok = VirtualFree(memory, 0, MEM_RELEASE);
				if (!free_ok)
					return false;
			#elif AEH_LINUX
				int const result = munmap(memory, virtual_region_size);
				if (result != 0)
					return false;
			#endif

			physical_memory_backed_size = 0;
			memory = nullptr;
			virtual_region_size = 0;
		}

		return true;
	}

} // namespace aeh
