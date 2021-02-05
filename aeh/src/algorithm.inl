namespace aeh
{

	template <typename C, typename T>
	constexpr size_t erase(C & c, T value)
	{
		auto it = std::remove(c.begin(), c.end(), value);
		auto r = std::distance(it, c.end());
		c.erase(it, c.end());
		return r;
	}

	template <typename C, typename Pred>
	constexpr size_t erase_if(C & c, Pred predicate)
	{
		auto it = std::remove_if(c.begin(), c.end(), [&predicate](auto const & x) noexcept -> bool { return predicate(x); });
		auto r = std::distance(it, c.end());
		c.erase(it, c.end());
		return r;
	}

	template <typename C, typename Pred>
	constexpr size_t update_then_erase_if(C & c, Pred predicate)
	{
		auto it = std::remove_if(c.begin(), c.end(), predicate);
		auto r = std::distance(it, c.end());
		c.erase(it, c.end());
		return r;
	}

} // namespace aeh
