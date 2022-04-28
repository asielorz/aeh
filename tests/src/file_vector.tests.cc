#include "file_vector.hh"
#include <catch2/catch.hpp>
#include <map>

using FakeFilesystem = std::map<std::filesystem::path, std::string>;

// Will "commit" contents to a string on destruction.
struct TestOstringstream : public std::ostringstream
{
	explicit TestOstringstream(std::string * string_to_write_to_) noexcept
		: std::ostringstream(std::ios::binary)
		, string_to_write_to(string_to_write_to_)
	{}

	TestOstringstream(TestOstringstream && other) noexcept
		: std::ostringstream(std::move(other))
	{
		string_to_write_to = other.string_to_write_to;
		other.string_to_write_to = nullptr;
	}

	~TestOstringstream()
	{
		if (string_to_write_to)
			*string_to_write_to = str();
	}

	std::string * string_to_write_to;
};

template <typename Key>
struct BasicTestFilesystemTrait
{
	using path_type = Key;

	BasicTestFilesystemTrait(std::map<Key, std::string> & m) noexcept
		: fake_filesystem(&m) 
	{}

	std::vector<Key> files() const
	{
		std::vector<Key> paths;
		paths.reserve(fake_filesystem->size());
		for (auto const & [path, content] : *fake_filesystem)
			paths.push_back(path);
		return paths;
	}

	auto remove(Key const & filename) const -> bool
	{
		return fake_filesystem->erase(filename) != 0;
	}

	auto open_to_read(Key const & filename) const -> std::optional<std::istringstream>
	{
		auto const it = fake_filesystem->find(filename);
		if (it == fake_filesystem->end())
			return std::nullopt;
		else
			return std::istringstream(it->second, std::ios::binary);
	}

	auto open_to_write(Key const & filename) const -> std::optional<TestOstringstream>
	{
		std::string & content = (*fake_filesystem)[filename];
		return TestOstringstream(&content);
	}

private:
	std::map<Key, std::string> * fake_filesystem;
};

using TestFilesystemTrait = BasicTestFilesystemTrait<std::filesystem::path>;

template <typename T>
struct TestLoadTrait
{
	std::optional<T> load(std::istream & input_stream) const 
	{
		T t;
		input_stream >> t;
		if (input_stream.fail())
			return std::nullopt;
		else
			return t;
	}

	bool save(std::ostream & output_stream, T content) const
	{
		output_stream << content;
		return output_stream.good();
	}
};

using TestFileVector = aeh::FileVector<int, TestFilesystemTrait, TestLoadTrait<int>>;

TEST_CASE("A file vector must be loaded from a base path and will contain an object for each file on that folder")
{
	FakeFilesystem fake_filesystem = {
		{"bar", "-5"},
		{"baz", "0"},
		{"foo", "3"},
		{"quux", "100000"},
	};

	auto v = TestFileVector::load(TestFilesystemTrait(fake_filesystem));

	REQUIRE(v.size() == 4);

	REQUIRE(v.filename_at(0) == "bar");
	REQUIRE(v[0] == -5);

	REQUIRE(v.filename_at(1) == "baz");
	REQUIRE(v[1] == 0);

	REQUIRE(v.filename_at(2) == "foo");
	REQUIRE(v[2] == 3);

	REQUIRE(v.filename_at(3) == "quux");
	REQUIRE(v[3] == 100'000);
}

TEST_CASE("Elements can be inserted to the vector. A new file is created in the filesystem for each element added")
{
	FakeFilesystem fake_filesystem = {
		{"bar", "-5"},
		{"baz", "0"},
		{"foo", "3"},
		{"quux", "100000"},
	};

	auto v = TestFileVector::load(TestFilesystemTrait(fake_filesystem));

	v.add(10, "hello");
	v.add(20, "how do you do");
	v.add(30, "some string");

	REQUIRE(v.size() == 7);

	REQUIRE(v.filename_at(4) == "hello");
	REQUIRE(v[4] == 10);
	REQUIRE(v.filename_at(5) == "how do you do");
	REQUIRE(v[5] == 20);
	REQUIRE(v.filename_at(6) == "some string");
	REQUIRE(v[6] == 30);
	
	REQUIRE(fake_filesystem.size() == 7);

	REQUIRE(fake_filesystem[v.filename_at(4)] == std::to_string(v[4]));
	REQUIRE(fake_filesystem[v.filename_at(5)] == std::to_string(v[5]));
	REQUIRE(fake_filesystem[v.filename_at(6)] == std::to_string(v[6]));
}

TEST_CASE("Elements can be removed from the vector. This also removes files from the filesystem")
{
	FakeFilesystem fake_filesystem = {
		{"bar", "-5"},
		{"baz", "0"},
		{"foo", "3"},
		{"quux", "100000"},
	};

	auto v = TestFileVector::load(TestFilesystemTrait(fake_filesystem));

	v.remove(2); // Remove foo

	REQUIRE(v.size() == 3);
	REQUIRE(fake_filesystem.size() == 3);

	REQUIRE(v.filename_at(0) == "bar");
	REQUIRE(v[0] == -5);

	REQUIRE(v.filename_at(1) == "baz");
	REQUIRE(v[1] == 0);

	REQUIRE(v.filename_at(2) == "quux");
	REQUIRE(v[2] == 100'000);

	REQUIRE(fake_filesystem["bar"] == std::to_string(-5));
	REQUIRE(fake_filesystem["baz"] == std::to_string(0));
	REQUIRE(fake_filesystem["quux"] == std::to_string(100'000));
	REQUIRE(fake_filesystem.contains("foo") == false);
}

TEST_CASE("A value can be replaced with another. This also updates the file")
{
	FakeFilesystem fake_filesystem = {
		{"bar", "-5"},
		{"baz", "0"},
		{"foo", "3"},
		{"quux", "100000"},
	};

	auto v = TestFileVector::load(TestFilesystemTrait(fake_filesystem));
	
	v.replace(1, -78);

	REQUIRE(v[1] == -78);
	REQUIRE(fake_filesystem["baz"] == std::to_string(-78));
}

TEST_CASE("A file vector can be converted to a span of const Ts")
{
	FakeFilesystem fake_filesystem = {
		{"bar", "-5"},
		{"baz", "0"},
		{"foo", "3"},
		{"quux", "100000"},
	};

	auto v = TestFileVector::load(TestFilesystemTrait(fake_filesystem));

	std::span<int const> s = v;
	REQUIRE(s[0] == -5);
	REQUIRE(s[1] == 0);
	REQUIRE(s[2] == 3);
	REQUIRE(s[3] == 100'000);
}

AEH_MSVC_WARNING_PUSH()
AEH_MSVC_WARNING_DISABLE(4505) // 'dummy_path': unreferenced local function has been removed
static std::filesystem::path dummy_path()
{
	return "";
}
AEH_MSVC_WARNING_POP()

TEST_CASE("FileVector can be instanced with the provided base path traits")
{
	using V1 = aeh::FileVector<int, aeh::ConstantBasePathTrait<dummy_path>, TestLoadTrait<int>>;
	using V2 = aeh::FileVector<int, aeh::VariableBasePathTrait, TestLoadTrait<int>>;
}

TEST_CASE("The path type of the filesystem may be defined by the trait. It doesn't need to be always std::filesystem::path")
{
	using IntBasedTestFileVector = aeh::FileVector<int, BasicTestFilesystemTrait<int>, TestLoadTrait<int>>;

	std::map<int, std::string> fake_filesystem = {
		{-4, "-5"},
		{0, "0"},
		{45, "3"},
		{256, "100000"},
	};

	auto v = IntBasedTestFileVector::load(BasicTestFilesystemTrait<int>(fake_filesystem));

	REQUIRE(v.size() == 4);

	REQUIRE(v.filename_at(0) == -4);
	REQUIRE(v[0] == -5);

	REQUIRE(v.filename_at(1) == 0);
	REQUIRE(v[1] == 0);

	REQUIRE(v.filename_at(2) == 45);
	REQUIRE(v[2] == 3);

	REQUIRE(v.filename_at(3) == 256);
	REQUIRE(v[3] == 100'000);
}
