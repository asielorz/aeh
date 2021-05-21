#pragma once

#include <string>
#include <concepts>

namespace aeh::json
{

	struct StringBuilder;
	struct ObjectBuilder;
	struct ArrayBuilder;

	struct ValueBuilder
	{
		void operator << (std::nullptr_t);
		void operator << (int32_t value);
		void operator << (uint32_t value);
		void operator << (int64_t value);
		void operator << (uint64_t value);
		void operator << (float value);
		void operator << (double value);
		void operator << (std::same_as<bool> auto value) { using namespace std::literals; debug_check_write_value(); json_string += (value ? "true"sv : "false"sv); json_string += ','; }
		void operator << (std::string_view value);

		[[nodiscard]] auto object() -> ObjectBuilder;
		[[nodiscard]] auto array() -> ArrayBuilder;

	private:
#ifdef NDEBUG
		[[nodiscard]] explicit ValueBuilder(std::string & s) noexcept : json_string(s) {}
		void debug_check_write_value() const noexcept {}
#else
		[[nodiscard]] explicit ValueBuilder(std::string & s, bool & parent) noexcept : what_to_pass_to_child_as_parent(parent), json_string(s) {}
		virtual void debug_check_write_value() = 0;
		bool & what_to_pass_to_child_as_parent;
#endif

	protected:
		std::string & json_string;

		friend struct ObjectMemberBuilder;
		friend struct ArrayBuilder;
	};

	template <typename T>
	concept JsonSerializable = requires(ValueBuilder & builder, T const & value)
	{
		builder << value;
	};

	struct ObjectMemberBuilder : ValueBuilder
	{
		ObjectMemberBuilder(ObjectMemberBuilder const &) = delete;
		ObjectMemberBuilder(ObjectMemberBuilder &&) = delete;

#ifndef NDEBUG
		~ObjectMemberBuilder();
#endif

	private:
#ifdef NDEBUG
		[[nodiscard]] explicit ObjectMemberBuilder(std::string & s) : ValueBuilder(s) {}
		void debug_check_write_value() const noexcept {}
#else
		[[nodiscard]] explicit ObjectMemberBuilder(std::string & s, bool & parent) : ValueBuilder(s, parent) {}
		void debug_check_write_value() override;
		bool single_value_written = false;
#endif

		friend struct StringBuilder;
		friend struct ObjectBuilder;
	};

	struct ObjectBuilder
	{
		[[nodiscard]] auto operator[] (std::string_view key) -> ObjectMemberBuilder;

		~ObjectBuilder();
		ObjectBuilder(ObjectBuilder const &) = delete;
		ObjectBuilder(ObjectBuilder &&) = delete;

	private:
#ifdef NDEBUG
		[[nodiscard]] explicit ObjectBuilder(std::string & s);
#else
		[[nodiscard]] explicit ObjectBuilder(std::string & s, bool & parent);
#endif

		std::string & json_string;
#ifndef NDEBUG
		bool parsing_child = false;
		bool & parent_parsing_child;
#endif

		friend struct ArrayBuilder;
		friend struct ValueBuilder;
	};

	struct ArrayBuilder : ValueBuilder
	{
		ArrayBuilder & operator << (JsonSerializable auto const & value) { static_cast<ValueBuilder &>(*this) << value; return *this; }

		~ArrayBuilder();
		ArrayBuilder(ObjectBuilder const &) = delete;
		ArrayBuilder(ObjectBuilder &&) = delete;

	private:
#ifdef NDEBUG
		[[nodiscard]] explicit ArrayBuilder(std::string & s);
#else
		[[nodiscard]] explicit ArrayBuilder(std::string & s, bool & parent);
#endif

#ifdef NDEBUG
		void debug_check_write_value() const noexcept {}
#else
		void debug_check_write_value() override;
#endif

#ifndef NDEBUG
		bool parsing_child = false;
		bool & parent_parsing_child;
#endif

		friend struct ValueBuilder;
	};

	struct StringBuilder : ObjectMemberBuilder
	{
		[[nodiscard]] explicit StringBuilder(std::string & s);
		~StringBuilder();

	private:
#ifndef NDEBUG
		bool parsing_child = false;
#endif
	};

	struct MultipleStringBuilder
	{
		[[nodiscard]] explicit MultipleStringBuilder(std::string & s, char separator_ = '\n') : json_string(s), separator(separator_) {}

		[[nodiscard]] auto next() -> StringBuilder;

	private:
		std::string & json_string;
		char separator;
	};

} // namespace json
