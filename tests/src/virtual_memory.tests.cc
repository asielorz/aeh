#include "virtual_memory.hh"
#include <catch2/catch.hpp>

TEST_CASE("Default constructed virtual memory region has no memory allocated")
{
	aeh::VirtualMemoryRegion default_constructed;

	REQUIRE(default_constructed.memory_region_address() == nullptr);

	REQUIRE(default_constructed.virtual_memory_size_in_bytes() == 0);
	REQUIRE(default_constructed.virtual_memory_size_in_pages() == 0);

	REQUIRE(default_constructed.physical_memory_size_in_bytes() == 0);
	REQUIRE(default_constructed.physical_memory_size_in_pages() == 0);
}

TEST_CASE("Allocating virtual memory does not allocate physical memory")
{
	aeh::VirtualMemoryRegion region = aeh::VirtualMemoryRegion::reserve_pages_of_virtual_memory(1);

	REQUIRE(region.memory_region_address() != nullptr);

	REQUIRE(region.virtual_memory_size_in_bytes() == aeh::os_memory_page_size);
	REQUIRE(region.virtual_memory_size_in_pages() == 1);

	// No physical memory has been reserved.
	REQUIRE(region.physical_memory_size_in_bytes() == 0);
	REQUIRE(region.physical_memory_size_in_pages() == 0);
}

TEST_CASE("Virtual memory to allocate can be specified in bytes. Size will be aligned to page size automatically")
{
	aeh::VirtualMemoryRegion region = aeh::VirtualMemoryRegion::reserve_bytes_of_virtual_memory(aeh::os_memory_page_size + 1);

	REQUIRE(region.memory_region_address() != nullptr);

	// Size in bytes has been aligned to 2 pages.
	REQUIRE(region.virtual_memory_size_in_bytes() == 2 * aeh::os_memory_page_size);
	REQUIRE(region.virtual_memory_size_in_pages() == 2);

	// No physical memory has been reserved.
	REQUIRE(region.physical_memory_size_in_bytes() == 0);
	REQUIRE(region.physical_memory_size_in_pages() == 0);
}

TEST_CASE("free_virtual_memory_region frees all resources and leaves the object back in the partially formed state")
{
	aeh::VirtualMemoryRegion region = aeh::VirtualMemoryRegion::reserve_pages_of_virtual_memory(1);

	REQUIRE(region.memory_region_address() != nullptr);
	REQUIRE(region.virtual_memory_size_in_bytes() == aeh::os_memory_page_size);
	REQUIRE(region.virtual_memory_size_in_pages() == 1);

	region.free_virtual_memory_region();

	// Virtual memory has been freed.
	REQUIRE(region.memory_region_address() == nullptr);
	REQUIRE(region.virtual_memory_size_in_bytes() == 0);
	REQUIRE(region.virtual_memory_size_in_pages() == 0);
}

TEST_CASE("VirtualMemoryRegion can be moved. The moved from state is the partially formed state.")
{
	aeh::VirtualMemoryRegion region = aeh::VirtualMemoryRegion::reserve_pages_of_virtual_memory(1);

	REQUIRE(region.memory_region_address() != nullptr);
	REQUIRE(region.virtual_memory_size_in_bytes() == aeh::os_memory_page_size);
	REQUIRE(region.virtual_memory_size_in_pages() == 1);

	void const * const virtual_memory_region_address = region.memory_region_address();
	aeh::VirtualMemoryRegion another_region = std::move(region);

	// `region` has no virtual memory now.
	REQUIRE(region.memory_region_address() == nullptr);
	REQUIRE(region.virtual_memory_size_in_bytes() == 0);
	REQUIRE(region.virtual_memory_size_in_pages() == 0);

	// `another_region` has the virtual memory that used to belong to `region`.
	REQUIRE(another_region.memory_region_address() == virtual_memory_region_address);
	REQUIRE(another_region.virtual_memory_size_in_bytes() == aeh::os_memory_page_size);
	REQUIRE(another_region.virtual_memory_size_in_pages() == 1);
}

TEST_CASE("VirtualMemoryRegion can be moved using the move assignment operator as well")
{
	aeh::VirtualMemoryRegion region = aeh::VirtualMemoryRegion::reserve_pages_of_virtual_memory(1);

	REQUIRE(region.memory_region_address() != nullptr);
	REQUIRE(region.virtual_memory_size_in_bytes() == aeh::os_memory_page_size);
	REQUIRE(region.virtual_memory_size_in_pages() == 1);

	void const * const virtual_memory_region_address = region.memory_region_address();
	aeh::VirtualMemoryRegion another_region;
	another_region = std::move(region);

	// `region` has no virtual memory now.
	REQUIRE(region.memory_region_address() == nullptr);
	REQUIRE(region.virtual_memory_size_in_bytes() == 0);
	REQUIRE(region.virtual_memory_size_in_pages() == 0);

	// `another_region` has the virtual memory that used to belong to `region`.
	REQUIRE(another_region.memory_region_address() == virtual_memory_region_address);
	REQUIRE(another_region.virtual_memory_size_in_bytes() == aeh::os_memory_page_size);
	REQUIRE(another_region.virtual_memory_size_in_pages() == 1);
}

TEST_CASE("allocate_extra_pages_of_physical_memory allocates physical memory within the virtual memory region")
{
	aeh::VirtualMemoryRegion region = aeh::VirtualMemoryRegion::reserve_pages_of_virtual_memory(16);
	region.allocate_extra_pages_of_physical_memory(4);

	REQUIRE(region.memory_region_address() != nullptr);

	REQUIRE(region.virtual_memory_size_in_bytes() == 16 * aeh::os_memory_page_size);
	REQUIRE(region.virtual_memory_size_in_pages() == 16);

	REQUIRE(region.physical_memory_size_in_bytes() == 4 * aeh::os_memory_page_size);
	REQUIRE(region.physical_memory_size_in_pages() == 4);

	// Physical memory region can be written to.
	memset(region.memory_region_address(), 0xDD, region.physical_memory_size_in_bytes());
}

TEST_CASE("allocate_extra_pages_of_physical_memory adds pages to the already existing")
{
	aeh::VirtualMemoryRegion region = aeh::VirtualMemoryRegion::reserve_pages_of_virtual_memory(16);
	REQUIRE(region.physical_memory_size_in_pages() == 0);

	region.allocate_extra_pages_of_physical_memory(2);
	REQUIRE(region.physical_memory_size_in_pages() == 2);
	
	region.allocate_extra_pages_of_physical_memory(2);
	REQUIRE(region.physical_memory_size_in_pages() == 4);

	region.allocate_extra_pages_of_physical_memory(2);
	REQUIRE(region.physical_memory_size_in_pages() == 6);

	region.allocate_extra_pages_of_physical_memory(2);
	REQUIRE(region.physical_memory_size_in_pages() == 8);
}

TEST_CASE("free_pages_of_physical_memory frees pages of physical memory from the back of the allocated region")
{
	aeh::VirtualMemoryRegion region = aeh::VirtualMemoryRegion::reserve_pages_of_virtual_memory(16);
	REQUIRE(region.physical_memory_size_in_pages() == 0);

	region.allocate_extra_pages_of_physical_memory(8);
	REQUIRE(region.physical_memory_size_in_pages() == 8);

	region.free_pages_of_physical_memory(2);
	REQUIRE(region.physical_memory_size_in_pages() == 6);

	region.free_pages_of_physical_memory(2);
	REQUIRE(region.physical_memory_size_in_pages() == 4);

	region.free_pages_of_physical_memory(2);
	REQUIRE(region.physical_memory_size_in_pages() == 2);

	region.free_pages_of_physical_memory(2);
	REQUIRE(region.physical_memory_size_in_pages() == 0);
}

TEST_CASE("Asking to free more pages than existing just frees all physical memory")
{
	aeh::VirtualMemoryRegion region = aeh::VirtualMemoryRegion::reserve_pages_of_virtual_memory(16);
	REQUIRE(region.physical_memory_size_in_pages() == 0);

	region.allocate_extra_pages_of_physical_memory(8);
	REQUIRE(region.physical_memory_size_in_pages() == 8);

	region.free_pages_of_physical_memory(200);
	REQUIRE(region.physical_memory_size_in_pages() == 0);
}

TEST_CASE("free_all_pages_of_physical_memory_up_to frees all physical memory except for the number of pages requested")
{
	aeh::VirtualMemoryRegion region = aeh::VirtualMemoryRegion::reserve_pages_of_virtual_memory(16);
	REQUIRE(region.physical_memory_size_in_pages() == 0);

	region.allocate_extra_pages_of_physical_memory(8);
	REQUIRE(region.physical_memory_size_in_pages() == 8);

	region.free_all_pages_of_physical_memory_up_to(2);
	REQUIRE(region.physical_memory_size_in_pages() == 2);

	// This operation is idempotent.
	region.free_all_pages_of_physical_memory_up_to(2);
	REQUIRE(region.physical_memory_size_in_pages() == 2);

	// Asking to keep more pages than existing is a noop
	region.free_all_pages_of_physical_memory_up_to(2000);
	REQUIRE(region.physical_memory_size_in_pages() == 2);
}
