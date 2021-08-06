#include "memory_arena.hh"
#include <catch2/catch.hpp>

TEST_CASE("Bytes can be allocated from a memory arena")
{
	auto arena = aeh::MemoryArena::with_virtual_region(1024 * 1024);

	int * const p = static_cast<int *>(arena.allocate(sizeof(int), alignof(int)));

	// Allocated memory can be written to.
	*p = -45;
	REQUIRE(arena.allocated_bytes() == sizeof(int));
}

TEST_CASE("Allocated memory can be freed in batch")
{
	auto arena = aeh::MemoryArena::with_virtual_region(1024 * 1024);

	int * const p1 = static_cast<int *>(arena.allocate(sizeof(int), alignof(int)));
	int * const p2 = static_cast<int *>(arena.allocate(sizeof(int), alignof(int)));
	int * const p3 = static_cast<int *>(arena.allocate(sizeof(int), alignof(int)));

	REQUIRE(arena.allocated_bytes() == 3 * sizeof(int));
	REQUIRE(p2 > p1);
	REQUIRE(p3 > p2);

	arena.free_all();

	REQUIRE(arena.allocated_bytes() == 0);
}

TEST_CASE("Freeing memory makes the arena reuse it for future allocations")
{
	auto arena = aeh::MemoryArena::with_virtual_region(1024 * 1024);

	int * const p_before = static_cast<int *>(arena.allocate(sizeof(int), alignof(int)));
	
	arena.free_all();

	int * const p_after = static_cast<int *>(arena.allocate(sizeof(int), alignof(int)));

	REQUIRE(p_before == p_after);
}

TEST_CASE("A MemoryArenaScopeGuard automatically frees all memory allocated while it was alive at scope exit")
{
	auto arena = aeh::MemoryArena::with_virtual_region(1024 * 1024);
	
	static_cast<int *>(arena.allocate(sizeof(int), alignof(int)));
	REQUIRE(arena.allocated_bytes() == sizeof(int));

	{
		auto const guard = aeh::MemoryArenaScopeGuard(arena);

		static_cast<int *>(arena.allocate(sizeof(int), alignof(int)));
		REQUIRE(arena.allocated_bytes() == 2 * sizeof(int));
	}
	// Destructor of `guard`

	REQUIRE(arena.allocated_bytes() == sizeof(int));
}
