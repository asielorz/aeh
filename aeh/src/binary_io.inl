namespace aeh
{

	template <typename InputStream, typename T>
	InputStream & read_binary(InputStream & is, T & t) noexcept
	{
		is.read(reinterpret_cast<char *>(std::addressof(t)), sizeof(T));
		return is;
	}

	template <typename InputStream, typename T>
	InputStream & read_binary(InputStream & is, T t[], int n) noexcept
	{
		is.read(reinterpret_cast<char *>(t), sizeof(T) * n);
		return is;
	}

	template <typename InputStream, typename T, typename Alloc>
	InputStream & read_binary(InputStream & is, std::vector<T, Alloc> & v) noexcept
	{
		is.read(reinterpret_cast<char *>(v.data()), sizeof(T) * v.size());
		return is;
	}

	template <typename OutputStream, typename T>
	OutputStream & write_binary(OutputStream & os, const T & t) noexcept
	{
		os.write(reinterpret_cast<const char *>(std::addressof(t)), sizeof(T));
		return os;
	}

	template <typename OutputStream, typename T>
	OutputStream & write_binary(OutputStream & os, const T t[], int n) noexcept
	{
		os.write(reinterpret_cast<const char *>(t), sizeof(T) * n);
		return os;
	}

	template <typename OutputStream, typename T, typename Alloc>
	OutputStream & write_binary(OutputStream & os, const std::vector<T, Alloc> & v) noexcept
	{
		os.write(reinterpret_cast<const char *>(v.data()), sizeof(T) * v.size());
		return os;
	}

	template <typename T, typename U>
	auto read_and_advance(U const *& p) noexcept -> T
	{
		auto const val = *reinterpret_cast<T const *>(p);
		reinterpret_cast<T const *&>(p)++;
		return val;
	}

	template <typename T, typename U>
	auto write_and_advance(U *& p, T const & val) noexcept -> void
	{
		*reinterpret_cast<T *>(p) = val;
		reinterpret_cast<T *&>(p)++;
	}

} // namespace aeh
