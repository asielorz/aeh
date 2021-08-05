#pragma once

namespace aeh
{

	extern size_t const os_memory_page_size;

	struct VirtualMemoryRegion
	{
		VirtualMemoryRegion() noexcept = default;
		VirtualMemoryRegion(VirtualMemoryRegion const &) = delete;
		VirtualMemoryRegion(VirtualMemoryRegion && other) noexcept;
		auto operator = (VirtualMemoryRegion const &) -> VirtualMemoryRegion & = delete;
		auto operator = (VirtualMemoryRegion && other) noexcept -> VirtualMemoryRegion &;
		~VirtualMemoryRegion();

		[[nodiscard]] static auto reserve_bytes_of_virtual_memory(size_t number_of_bytes) noexcept -> VirtualMemoryRegion;
		[[nodiscard]] static auto reserve_pages_of_virtual_memory(size_t number_of_pages) noexcept -> VirtualMemoryRegion;

		[[nodiscard]] auto memory_region_address() const noexcept -> void * { return memory; }
		[[nodiscard]] auto physical_memory_size_in_bytes() const noexcept -> size_t { return physical_memory_backed_size; }
		[[nodiscard]] auto physical_memory_size_in_pages() const noexcept -> size_t { return physical_memory_backed_size / os_memory_page_size; }
		[[nodiscard]] auto virtual_memory_size_in_bytes() const noexcept -> size_t { return virtual_region_size; }
		[[nodiscard]] auto virtual_memory_size_in_pages() const noexcept -> size_t { return virtual_region_size / os_memory_page_size; }

		// Return false and leave the class unchanged when the underlying system call fails.
		auto allocate_extra_pages_of_physical_memory(size_t number_of_pages) noexcept -> bool;
		auto free_pages_of_physical_memory(size_t number_of_pages) noexcept -> bool;
		auto free_all_pages_of_physical_memory_up_to(size_t number_of_pages_to_keep) noexcept -> bool;

		// Free all resources and put the object into default-constructed/moved-from state. This state is partially formed.
		auto free_virtual_memory_region() noexcept -> bool;

	private:
		void * memory = nullptr;
		size_t physical_memory_backed_size = 0;
		size_t virtual_region_size = 0;
	};

} // namespace aeh
