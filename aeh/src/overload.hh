#pragma once

#include <type_traits>

namespace aeh
{

	template <typename ... Ts>
	struct overload : public Ts...
	{
		overload(Ts ... ts) : Ts(ts)... {}
		using Ts::operator()...;
	};

} // namespace aeh
