#pragma once

#include <cstdint>
#include <span>
#include <ios>

namespace aeh
{

	//! Reads sizeof(T) bytes from is into t.
	template <typename InputStream, typename T, typename> InputStream & read_binary(InputStream & is, T & t) noexcept;
	//! Reads sizeof(T) * n bytes from os into an array pointed to by t.
	template <typename InputStream, typename T, typename> InputStream & read_binary(InputStream & is, T t[], int n) noexcept;
	//! Reads sizeof(T) * t.size() bytes from os into an array pointed to by t.
	template <typename InputStream, typename T, typename> InputStream & read_binary(InputStream & is, std::span<T> t) noexcept;
	//! Writes sizeof(T) bytes from t into os.
	template <typename OutputStream, typename T, typename> OutputStream & write_binary(OutputStream & os, T const & t) noexcept;
	//! Writes sizeof(T) * n bytes from an array pointed to by t into os.
	template <typename OutputStream, typename T, typename> OutputStream & write_binary(OutputStream & os, T const t[], int n) noexcept;
	//! Writes sizeof(T) * t.size() bytes from an array pointed to by t into os.
	template <typename OutputStream, typename T, typename> OutputStream & write_binary(OutputStream & os, std::span<T const> t) noexcept;
	//! Reads sizeof(T) bytes from p and advances p by sizeof(T) bytes.
	template <typename T, typename U> auto read_and_advance(U const *& p) noexcept -> T;
	//! Writes val (sizeof(T) bytes) to p and advances p by sizeof(T) bytes.
	template <typename T, typename U> auto write_and_advance(U *& p, T const & val) noexcept -> void;

	//! Reads a file written by write_binary_file from is. The function fails if the stream does not contain
	//! the correct amount of bytes, or if the header and the checksum do not match.
	template <typename InputStream, typename T, typename FileIdentifierHeader, typename> 
	auto read_binary_file(InputStream & is, T & t, FileIdentifierHeader const & expected_file_identifier) noexcept -> bool;
	//! Writes a trivial object into os, as well as a header to ensure the integrity when reading, containing an identifier and a checksum.
	template <typename OutputStream, typename T, typename FileIdentifierHeader, typename>
	auto write_binary_file(OutputStream & os, T const & t, FileIdentifierHeader const & file_identifier) noexcept -> void;

	//! Returns the number of bytes left to read in a stream.
	template <typename InputStream> auto bytes_remaining(InputStream & file) noexcept -> int64_t;

	//! Computes the checksum of an array of bytes by adding all the elements as 8 byte words.
	[[nodiscard]] auto checksum(void const * data, size_t size) noexcept -> uint64_t;
	//! Computes the checksum of the bytes a trivial object.
	template <typename T, typename> [[nodiscard]] auto checksum(T const & t) noexcept -> uint64_t;

} // namespace aeh

#include "binary_io.inl"
