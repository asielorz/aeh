#pragma once

#include <vector>

namespace aeh
{

	//! Reads sizeof(T) bytes from is into t.
	template <typename InputStream, typename T> InputStream & read_binary(InputStream & is, T & t) noexcept;
	//! Reads sizeof(T) * n bytes from os into an array pointed to by t.
	template <typename InputStream, typename T> InputStream & read_binary(InputStream & is, T t[], int n) noexcept;
	//! Reads sizeof(T) * v.size() bytes from is into v.data().
	template <typename InputStream, typename T, typename Alloc> InputStream & read_binary(InputStream & is, std::vector<T, Alloc> & v) noexcept;
	//! Writes sizeof(T) bytes from t into os.
	template <typename OutputStream, typename T> OutputStream & write_binary(OutputStream & os, const T & t) noexcept;
	//! Writes sizeof(T) * n bytes from an array pointed to by t into os.
	template <typename OutputStream, typename T> OutputStream & write_binary(OutputStream & os, const T t[], int n) noexcept;
	//! Writes sizeof(T) * v.size() bytes from v.data() into os.
	template <typename OutputStream, typename T, typename Alloc> OutputStream & write_binary(OutputStream & os, const std::vector<T, Alloc> & v) noexcept;
	//! Reads sizeof(T) bytes from p and advances p by sizeof(T) bytes.
	template <typename T, typename U> auto read_and_advance(U const *& p) noexcept -> T;
	//! Writes val (sizeof(T) bytes) to p and advances p by sizeof(T) bytes.
	template <typename T, typename U> auto write_and_advance(U *& p, T const & val) noexcept -> void;

} // namespace aeh

#include "binary_io.inl"
