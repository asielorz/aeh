#include "binary_io.hh"
#include <numeric>
#include <fstream>

namespace aeh
{

	auto checksum(void const * data, size_t size) noexcept -> uint64_t
	{
		size_t const word_count = size / sizeof(uint64_t);
		size_t const remaining_bytes = size % sizeof(uint64_t);
		uint64_t const * const data_as_words = static_cast<uint64_t const *>(data);

		size_t const sum_of_all_words = std::reduce(data_as_words, data_as_words + word_count);

		size_t word_from_remaining_bytes = 0;
		memcpy(&word_from_remaining_bytes, data_as_words + word_count, remaining_bytes);

		return sum_of_all_words + word_from_remaining_bytes;
	}

} // namespace aeh
