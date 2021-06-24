#pragma once

#include <type_traits>
#include <cstddef>

namespace aeh
{

	template <typename Pointee, typename ExtraData>
	struct PointerUnion
	{
		static_assert(sizeof(Pointee *) == 8, "PointerUnion only available in 64 bit architecture");
		static_assert(sizeof(ExtraData) == 2);
		static_assert(std::is_trivially_copyable_v<ExtraData>);

	private:
		struct ExtraDataHelper
		{
			unsigned short unused[3];
			ExtraData extra_data;
		};
	public:

		constexpr PointerUnion() noexcept : pointer_and_extra_data(nullptr) {}
		constexpr PointerUnion(std::nullptr_t) noexcept : pointer_and_extra_data(nullptr) {}
		constexpr PointerUnion(PointerUnion const &) noexcept = default;
		constexpr explicit PointerUnion(Pointee * p) noexcept : pointer_and_extra_data(p) {}
		constexpr explicit PointerUnion(Pointee * p, ExtraData s) noexcept : pointer_and_extra_data(p) { set_extra_data(s); }
		constexpr explicit PointerUnion(ExtraData s) noexcept : pointer_and_extra_data(nullptr) { set_extra_data(s); }
		constexpr explicit PointerUnion(size_t value) noexcept : pointer_and_extra_data(reinterpret_cast<Pointee *>(value)) {}

		// All pointers are convertible from owner_ptr<byte>.
		template <typename Byte, typename = std::enable_if_t<std::is_same_v<std::remove_const_t<Byte>, std::byte>>>
		constexpr explicit PointerUnion(PointerUnion<Byte, ExtraData> other) noexcept : PointerUnion(other.value()) {}

		static constexpr size_t pointer_mask = 0b00000000'00000000'11111111'11111111'11111111'11111111'11111111'11111111;
		static constexpr size_t extra_data_mask = ~pointer_mask;

		constexpr auto value() const noexcept -> size_t { return size_t(pointer_and_extra_data); }
		constexpr auto pointer() const noexcept -> Pointee * { return reinterpret_cast<Pointee *>(value() & pointer_mask); }
		constexpr auto extra_data() const noexcept -> ExtraData { return reinterpret_cast<ExtraDataHelper const &>(pointer_and_extra_data).extra_data; }

		constexpr auto set_pointer(Pointee * p) -> PointerUnion &
		{
			using PointeePtr = Pointee *; // Otherwise gcc 10.2.0 gets confused
			pointer_and_extra_data = PointeePtr((value() & extra_data_mask) | size_t(p));
			return *this;
		}
		constexpr auto set_extra_data(ExtraData s) -> PointerUnion &
		{
			reinterpret_cast<ExtraDataHelper &>(pointer_and_extra_data).extra_data = s;
			return *this;
		}
		constexpr auto set_value(size_t v) -> PointerUnion &
		{
			pointer_and_extra_data = reinterpret_cast<Pointee *>(v);
			return *this;
		}

		constexpr auto operator * () const noexcept -> Pointee & { return *pointer(); }
		constexpr auto operator -> () const noexcept -> Pointee * { return pointer(); }
		constexpr auto operator [] (size_t index) const noexcept -> Pointee & { return pointer()[index]; }
		constexpr explicit operator bool() const noexcept { return pointer() != nullptr; }
		constexpr operator PointerUnion<std::byte, ExtraData>() const noexcept { return PointerUnion<std::byte, ExtraData>(value()); }

	private:
		Pointee * pointer_and_extra_data;
	};

	template <typename Pointee, typename ExtraData>
	constexpr auto operator == (PointerUnion<Pointee, ExtraData> a, PointerUnion<Pointee, ExtraData> b) noexcept -> bool
	{
		return a.value() == b.value();
	}

	template <typename Pointee, typename ExtraData>
	constexpr auto operator != (PointerUnion<Pointee, ExtraData> a, PointerUnion<Pointee, ExtraData> b) noexcept -> bool
	{
		return !(a == b);
	}

} // namespace aeh
