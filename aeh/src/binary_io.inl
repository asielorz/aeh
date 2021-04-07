namespace aeh
{

	template <typename InputStream, typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	InputStream & read_binary(InputStream & is, T & t) noexcept
	{
		is.read(reinterpret_cast<char *>(std::addressof(t)), sizeof(T));
		return is;
	}

	template <typename InputStream, typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	InputStream & read_binary(InputStream & is, T t[], int n) noexcept
	{
		is.read(reinterpret_cast<char *>(t), sizeof(T) * n);
		return is;
	}
	
	template <typename InputStream, typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	InputStream & read_binary(InputStream & is, span<T> t) noexcept
	{
		return read_binary(is, t.data(), t.size());
	}

	template <typename OutputStream, typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	OutputStream & write_binary(OutputStream & os, T const & t) noexcept
	{
		os.write(reinterpret_cast<const char *>(std::addressof(t)), sizeof(T));
		return os;
	}

	template <typename OutputStream, typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	OutputStream & write_binary(OutputStream & os, T const t[], int n) noexcept
	{
		os.write(reinterpret_cast<const char *>(t), sizeof(T) * n);
		return os;
	}

	template <typename OutputStream, typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	OutputStream & write_binary(OutputStream & os, span<T const> t) noexcept
	{
		return write_binary(os, t.data(), t.size());
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

	template <typename InputStream, typename T, typename FileIdentifierHeader, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	auto read_binary_file(InputStream & is, T & t, FileIdentifierHeader const & expected_file_identifier) noexcept -> bool
	{
		FileIdentifierHeader file_identifier;
		aeh::read_binary(is, file_identifier);
		if (file_identifier != expected_file_identifier)
			return false;

		uint64_t file_checksum;
		aeh::read_binary(is, file_checksum);

		if (aeh::bytes_remaining(is) != sizeof(T))
			return false;

		T loaded_t;
		aeh::read_binary(is, loaded_t);

		if (file_checksum != aeh::checksum(loaded_t))
			return false;

		t = loaded_t;
		return true;
	}

	template <typename OutputStream, typename T, typename FileIdentifierHeader, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	auto write_binary_file(OutputStream & os, T const & t, FileIdentifierHeader const & file_identifier) noexcept -> void
	{
		aeh::write_binary(os, file_identifier);
		aeh::write_binary(os, checksum(t));
		aeh::write_binary(os, t);
	}

	template <typename InputStream>
	auto bytes_remaining(InputStream & file) noexcept -> int64_t
	{
		int64_t const current_position = file.tellg();
		file.seekg(0, std::ios::end);
		int64_t const end_position = file.tellg();
		file.seekg(current_position);
		return end_position - current_position;
	}

	template <typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	auto checksum(T const & t) noexcept -> uint64_t
	{
		return checksum(std::addressof(t), sizeof(T));
	}

} // namespace aeh
