#include "json_string_builder.hh"
#include <catch2/catch.hpp>

TEST_CASE("Root integer")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		builder << -4;
	}
	REQUIRE(str == "-4");
}

TEST_CASE("Root float")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		builder << 2.25;
	}
	REQUIRE(str == "2.250000");
}

TEST_CASE("Root bool")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		builder << false;
	}
	REQUIRE(str == "false");
}

TEST_CASE("Root null")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		builder << nullptr;
	}
	REQUIRE(str == "null");
}

TEST_CASE("Root string")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		builder << "Hello, world!";
	}
	REQUIRE(str == "\"Hello, world!\"");
}

TEST_CASE("Escaping characters")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		builder << "\t\n\r\"";
	}
	REQUIRE(str == "\"\\t\\n\\r\\\"\"");
}

TEST_CASE("Array root")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		auto array = builder.array();
		array << 4;
		array << 25;
		array << true;
		array << nullptr;
	}
	REQUIRE(str == "[4,25,true,null]");
}

TEST_CASE("Object root")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		auto object = builder.object();
		object["foo"] << 4;
		object["bar"] << true;
		object["quux"] << nullptr;
	}
	REQUIRE(str == R"({"foo":4,"bar":true,"quux":null})");
}

TEST_CASE("Subobject")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		auto object = builder.object();
		object["foo"] << 4;
		object["bar"] << true;
		{
			auto subobject = object["quux"].object();
			subobject["m1"] << nullptr;
			subobject["m2"] << "Hello!";
		}
	}
	REQUIRE(str == R"({"foo":4,"bar":true,"quux":{"m1":null,"m2":"Hello!"}})");
}

TEST_CASE("Array inside object")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		auto object = builder.object();
		object["foo"] << 4;
		object["bar"] << true;
		{
			auto subarray = object["quux"].array();
			subarray << nullptr;
			subarray << "Hello!";
		}
	}
	REQUIRE(str == R"({"foo":4,"bar":true,"quux":[null,"Hello!"]})");
}

TEST_CASE("Object inside array")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		auto array = builder.array();
		array << 4;
		array << 25;
		array << true;
		{
			auto object = array.object();
			object["m1"] << nullptr;
			object["m2"] << -10;
		}
	}
	REQUIRE(str == R"([4,25,true,{"m1":null,"m2":-10}])");
}

TEST_CASE("Several newline separated values in a string")
{
	std::string str;
	for (int i = 0; i < 4; ++i)
	{
		{
			auto builder = aeh::json::StringBuilder(str);
			auto array = builder.array();
			array << i;
			array << 25;
			array << true;
			array << nullptr;
		}
		str += '\n';
	}

	constexpr std::string_view expected = 
		"[0,25,true,null]\n"
		"[1,25,true,null]\n"
		"[2,25,true,null]\n"
		"[3,25,true,null]\n";

	REQUIRE(str == expected);
}

TEST_CASE("Several newline separated values in a string with an abstraction for doing that")
{
	std::string str;

	for (int i = 0; i < 4; ++i)
	{
		auto builder = aeh::json::MultipleStringBuilder(str, '\n').next();
		auto array = builder.array();
		array << i;
		array << 25;
		array << true;
		array << nullptr;
	}

	constexpr std::string_view expected =
		"[0,25,true,null]\n"
		"[1,25,true,null]\n"
		"[2,25,true,null]\n"
		"[3,25,true,null]";

	REQUIRE(str == expected);
}

struct test_vec3
{
	int x, y, z;

	friend void operator << (aeh::json::ValueBuilder & builder, test_vec3 v)
	{
		auto object = builder.object();
		object["x"] << v.x;
		object["y"] << v.y;
		object["z"] << v.z;
	}
};

TEST_CASE("Overloading <<")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		builder << test_vec3{1, 2, 3};
	}
	REQUIRE(str == R"({"x":1,"y":2,"z":3})");
}

TEST_CASE("Overloading << also works for arrays")
{
	std::string str;
	{
		auto builder = aeh::json::StringBuilder(str);
		auto array = builder.array();
		array << test_vec3{1, 2, 3}
			  << test_vec3{4, 5, 6}
			  << test_vec3{7, 8, 9};
	}
	REQUIRE(str == R"([{"x":1,"y":2,"z":3},{"x":4,"y":5,"z":6},{"x":7,"y":8,"z":9}])");
}
