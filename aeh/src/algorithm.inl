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

	template <typename It> requires std::forward_iterator<It> && requires(It a) { *a + *a; }
	constexpr auto sum(It begin, It end) noexcept -> std::decay_t<decltype(*begin)>
	{
		using value_type = std::decay_t<decltype(*begin)>;
		return std::accumulate(begin, end, value_type(0));
	}

	template <typename R>
	constexpr auto sum(R && range) noexcept -> decltype(sum(std::begin(range), std::end(range)))
	{
		return sum(std::begin(range), std::end(range));
	}

} // namespace aeh
