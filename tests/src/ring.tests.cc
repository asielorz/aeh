#include "catch2/catch.hpp"
#include "ring.hh"
#include <queue>

namespace tests
{
	// Compares if a ring contains the required values
	template <typename T> requires std::equality_comparable<T>
	bool contains_exactly(aeh::ring<T> ring, std::initializer_list<T> values)
	{
		if (ring.size() != values.size())
		{
			return false;
		}

		for (T const& value : values)
		{
			if (value != ring.front())
			{
				return false;
			}

			ring.pop_front();
		}

		return true;
	}
}

static_assert(std::is_same_v<int, aeh::ring<int>::value_type>, "Value type of ring<int> is int");
static_assert(std::is_same_v<double, aeh::ring<double>::value_type>, "Value type of ring<double> is double");

TEST_CASE("Default constructed ring is empty and allocates no memory")
{
	aeh::ring<int> ring;

	REQUIRE(ring.empty());
	REQUIRE(ring.size() == 0);
	REQUIRE(ring.capacity() == 0);
}

TEST_CASE("push_back increases size by 1")
{
	aeh::ring<int> ring;

	ring.push_back(5);

	REQUIRE(ring.size() == 1);

	ring.push_back(7);

	REQUIRE(ring.size() == 2);
}

TEST_CASE("push_back returns a reference to the inserted element")
{
	aeh::ring<int> ring;

	int const& n1 = ring.push_back(5);

	REQUIRE(ring.size() == 1);
	REQUIRE(n1 == 5);

	int const& n2 = ring.push_back(7);

	REQUIRE(ring.size() == 2);
	REQUIRE(n2 == 7);
}

TEST_CASE("back returns a reference to the last inserted element")
{
	aeh::ring<int> ring;

	int const& n1 = ring.push_back(5);

	REQUIRE(ring.size() == 1);
	REQUIRE(ring.back() == 5);
	REQUIRE(n1 == ring.back());
	REQUIRE(&n1 == &ring.back());

	int const& n2 = ring.push_back(7);

	REQUIRE(ring.size() == 2);
	REQUIRE(ring.back() == 7);
	REQUIRE(n2 == ring.back());
	REQUIRE(&n2 == &ring.back());
}

TEST_CASE("pop_front on a ring with size 1 leaves it empty")
{
	aeh::ring<int> ring;

	ring.push_back(5);

	REQUIRE(ring.size() == 1);

	ring.pop_front();

	REQUIRE(ring.size() == 0);
}

TEST_CASE("pop_front does not affect capacity")
{
	aeh::ring<int> ring;

	ring.push_back(5);
	size_t const capacity_before = ring.capacity();

	REQUIRE(capacity_before > 0);

	ring.pop_front();
	size_t const capacity_after = ring.capacity();

	REQUIRE(capacity_before == capacity_after);
}

TEST_CASE("pop_front removes the first element pushed")
{
	aeh::ring<int> ring;

	ring.push_back(5);
	ring.push_back(7);

	REQUIRE(ring.front() == 5);
	REQUIRE(ring.back() == 7);

	ring.pop_front();

	REQUIRE(ring.front() == 7);
	REQUIRE(ring.back() == 7);
}

TEST_CASE("reserve increases capacity to the given value, has no effect on size")
{
	aeh::ring<int> ring;
	ring.reserve(32);

	REQUIRE(ring.size() == 0);
	REQUIRE(ring.capacity() == 32);
}

TEST_CASE("If reserve would decrease capacity, it has no effect instead")
{
	aeh::ring<int> ring;
	ring.reserve(32);

	REQUIRE(ring.size() == 0);
	REQUIRE(ring.capacity() == 32);

	ring.reserve(16);

	REQUIRE(ring.size() == 0);
	REQUIRE(ring.capacity() == 32); // Capacity is still 32
}

TEST_CASE("shrink_to_fit reduces capacity to match size")
{
	aeh::ring<int> ring;
	ring.reserve(32);

	ring.push_back(15);
	ring.push_back(16);

	REQUIRE(ring.size() == 2);
	REQUIRE(ring.capacity() == 32);
	REQUIRE(ring.front() == 15);
	REQUIRE(ring.back() == 16);

	ring.shrink_to_fit();

	REQUIRE(ring.size() == 2);
	REQUIRE(ring.capacity() == 2);
	REQUIRE(ring.front() == 15);
	REQUIRE(ring.back() == 16);
}

TEST_CASE("Filling to full capacacity, then popping, then pushing in the space left, capacity does not change")
{
	aeh::ring<int> ring;
	ring.reserve(8);

	for (int i = 0; i < 8; ++i)
		ring.push_back(i);

	REQUIRE(ring.size() == 8);
	REQUIRE(ring.capacity() == 8);
	REQUIRE(ring.front() == 0);
	REQUIRE(ring.back() == 7);

	ring.pop_front();
	ring.push_back(8);

	REQUIRE(ring.size() == 8);
	REQUIRE(ring.capacity() == 8);
	REQUIRE(ring.front() == 1);
	REQUIRE(ring.back() == 8);
}

TEST_CASE("clear removes all elements, preserves capacity")
{
	aeh::ring<int> ring;

	ring.push_back(4);
	ring.push_back(6);

	size_t const capacity_before = ring.capacity();

	REQUIRE(ring.size() == 2);
	REQUIRE(ring.front() == 4);
	REQUIRE(ring.back() == 6);
	REQUIRE(capacity_before >= 2);

	ring.clear();

	REQUIRE(ring.size() == 0);
	REQUIRE(ring.capacity() == capacity_before);
}

TEST_CASE("reset removes all elements and frees allocated memory")
{
	aeh::ring<int> ring;

	ring.push_back(4);
	ring.push_back(6);

	REQUIRE(ring.size() == 2);
	REQUIRE(ring.front() == 4);
	REQUIRE(ring.back() == 6);
	REQUIRE(ring.capacity() >= 2);

	ring.reset();

	REQUIRE(ring.size() == 0);
	REQUIRE(ring.capacity() == 0);
}

TEST_CASE("Move constructor constructs a ring with the same value as another, and leaves the original empty")
{
	aeh::ring<int> ring;

	ring.push_back(4);
	ring.push_back(6);

	REQUIRE(ring.size() == 2);
	REQUIRE(ring.front() == 4);
	REQUIRE(ring.back() == 6);
	REQUIRE(ring.capacity() >= 2);

	aeh::ring<int> const copy = std::move(ring);

	REQUIRE(copy.size() == 2);
	REQUIRE(copy.front() == 4);
	REQUIRE(copy.back() == 6);
	REQUIRE(copy.capacity() >= 2);

	REQUIRE(ring.size() == 0);
	REQUIRE(ring.capacity() == 0);
}

TEST_CASE("Move assignment constructs a ring with the same value as another, and leaves the original empty")
{
	aeh::ring<int> ring;

	ring.push_back(4);
	ring.push_back(6);

	REQUIRE(ring.size() == 2);
	REQUIRE(ring.front() == 4);
	REQUIRE(ring.back() == 6);
	REQUIRE(ring.capacity() >= 2);

	aeh::ring<int> copy;
	copy = std::move(ring);

	REQUIRE(copy.size() == 2);
	REQUIRE(copy.front() == 4);
	REQUIRE(copy.back() == 6);
	REQUIRE(copy.capacity() >= 2);

	REQUIRE(ring.size() == 0);
	REQUIRE(ring.capacity() == 0);
}

TEST_CASE("Copy constructor constructs a ring with the same value as another, and leaves the original unchanged")
{
	aeh::ring<int> ring;

	ring.push_back(4);
	ring.push_back(6);

	REQUIRE(ring.size() == 2);
	REQUIRE(ring.front() == 4);
	REQUIRE(ring.back() == 6);
	REQUIRE(ring.capacity() >= 2);

	aeh::ring<int> const copy = std::as_const(ring);

	REQUIRE(ring.size() == 2);
	REQUIRE(ring.front() == 4);
	REQUIRE(ring.back() == 6);
	REQUIRE(ring.capacity() >= 2);

	REQUIRE(copy.size() == 2);
	REQUIRE(copy.front() == 4);
	REQUIRE(copy.back() == 6);
	REQUIRE(copy.capacity() >= 2);
}

TEST_CASE("Copy assignment constructs a ring with the same value as another, and leaves the original unchanged")
{
	aeh::ring<int> ring;

	ring.push_back(4);
	ring.push_back(6);

	REQUIRE(ring.size() == 2);
	REQUIRE(ring.front() == 4);
	REQUIRE(ring.back() == 6);
	REQUIRE(ring.capacity() >= 2);

	aeh::ring<int> copy;
	copy = std::as_const(ring);

	REQUIRE(ring.size() == 2);
	REQUIRE(ring.front() == 4);
	REQUIRE(ring.back() == 6);
	REQUIRE(ring.capacity() >= 2);

	REQUIRE(copy.size() == 2);
	REQUIRE(copy.front() == 4);
	REQUIRE(copy.back() == 6);
	REQUIRE(copy.capacity() >= 2);
}

TEST_CASE("If reserve is not called, more capacity is allocated as push_back is called")
{
	aeh::ring<int> ring;

	for (int i = 0; i < 256; ++i)
		ring.push_back(i);

	REQUIRE(ring.capacity() >= 256);

	for (int i = 0; i < 256; ++i)
	{
		REQUIRE(ring.front() == i);
		ring.pop_front();
	}
}

TEST_CASE("ring can be used as the underlying container for a std::queue")
{
	using queue_type = std::queue<int, aeh::ring<int>>;

	queue_type queue;

	for (int i = 0; i < 256; ++i)
		queue.push(i);

	for (int i = 0; i < 256; ++i)
	{
		REQUIRE(queue.front() == i);
		queue.pop();
	}
}

consteval int test_constexpr_ring()
{
	aeh::ring<int> ring;

	ring.reserve(32);

	ring.push_back(1);
	ring.push_back(2);
	ring.push_back(3);
	ring.push_back(4);

	ring.clear();

	ring.push_back(1);
	ring.push_back(2);
	ring.push_back(3);
	ring.push_back(4);

	ring.reset();

	ring.push_back(1);
	ring.push_back(2);
	ring.push_back(3);
	ring.push_back(4);

	ring.pop_front();
	ring.pop_front();

	return ring.front() + ring.back();
}

static_assert(test_constexpr_ring() == 7, "API of aeh::ring can be used at compile time");
