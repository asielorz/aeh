#include "file_vector.hh"
#include <catch2/catch.hpp>
#include <map>

template <typename T>
using FakeFilesystem = std::map<std::string, T>;

template <typename T>
struct TestBasePathTrait
{
	TestBasePathTrait(FakeFilesystem<T> & m, std::string p = "") noexcept : fake_filesystem(&m), base_path_(std::move(p)) {}

	std::string base_path() const { return base_path_; }
	std::vector<std::string> files_in_base_path() const 
	{
		std::vector<std::string> paths;
		paths.reserve(fake_filesystem->size());
		for (auto const & [path, content] : *fake_filesystem)
			paths.push_back(path);
		return paths;
	}

private:
	std::string base_path_;
	FakeFilesystem<T> * fake_filesystem;
};

template <typename T>
struct TestLoadTrait
{
	TestLoadTrait(FakeFilesystem<T> & m) noexcept : fake_filesystem(&m) {}

	std::optional<T> load(std::string const & path) const 
	{
		auto const it = fake_filesystem->find(path);
		if (it != fake_filesystem->end())
			return it->second;
		else
			return std::nullopt;
	}

	bool save(std::string const & path, T content) const 
	{
		(*fake_filesystem)[path] = content;
		return true;
	}

	bool remove(std::string const & path) const
	{
		return fake_filesystem->erase(path) == 1;
	}

private:
	FakeFilesystem<T> * fake_filesystem;
};

using TestFileVector = aeh::FileVector<int, TestBasePathTrait<int>, TestLoadTrait<int>>;

TEST_CASE("A file vector must be loaded from a base path and will contain an object for each file on that folder")
{
	FakeFilesystem<int> fake_filesystem = {
		{"bar", -5},
		{"baz", 0},
		{"foo", 3},
		{"quux", 100'000},
	};

	auto v = TestFileVector::load(TestBasePathTrait<int>(fake_filesystem), TestLoadTrait<int>(fake_filesystem));
	std::span<aeh::File<int> const> const files = v.files();

	REQUIRE(files.size() == 4);

	REQUIRE(files[0].path == "bar");
	REQUIRE(files[0].content == -5);

	REQUIRE(files[1].path == "baz");
	REQUIRE(files[1].content == 0);

	REQUIRE(files[2].path == "foo");
	REQUIRE(files[2].content == 3);

	REQUIRE(files[3].path == "quux");
	REQUIRE(files[3].content == 100'000);
}

TEST_CASE("Elements can be inserted to the vector. A new file is created in the filesystem for each element added")
{
	FakeFilesystem<int> fake_filesystem = {
		{"bar", -5},
		{"baz", 0},
		{"foo", 3},
		{"quux", 100'000},
	};

	auto v = TestFileVector::load(TestBasePathTrait<int>(fake_filesystem), TestLoadTrait<int>(fake_filesystem));

	v.add(10, "hello");
	v.add(20, "how do you do");
	v.add(30, "some string");

	std::span<aeh::File<int> const> const files = v.files();

	REQUIRE(files.size() == 7);

	REQUIRE(files[4].path == "hello");
	REQUIRE(files[4].content == 10);
	REQUIRE(files[5].path == "how do you do");
	REQUIRE(files[5].content == 20);
	REQUIRE(files[6].path == "some string");
	REQUIRE(files[6].content == 30);
	
	REQUIRE(fake_filesystem.size() == 7);

	REQUIRE(fake_filesystem[files[4].path] == files[4].content);
	REQUIRE(fake_filesystem[files[5].path] == files[5].content);
	REQUIRE(fake_filesystem[files[6].path] == files[6].content);
}

TEST_CASE("Elements can be removed from the vector. This also removes files from the filesystem")
{
	FakeFilesystem<int> fake_filesystem = {
		{"bar", -5},
		{"baz", 0},
		{"foo", 3},
		{"quux", 100'000},
	};

	auto v = TestFileVector::load(TestBasePathTrait<int>(fake_filesystem), TestLoadTrait<int>(fake_filesystem));

	v.remove(2); // Remove foo

	std::span<aeh::File<int> const> const files = v.files();

	REQUIRE(files.size() == 3);
	REQUIRE(fake_filesystem.size() == 3);

	REQUIRE(files[0].path == "bar");
	REQUIRE(files[0].content == -5);

	REQUIRE(files[1].path == "baz");
	REQUIRE(files[1].content == 0);

	REQUIRE(files[2].path == "quux");
	REQUIRE(files[2].content == 100'000);

	REQUIRE(fake_filesystem["bar"] == -5);
	REQUIRE(fake_filesystem["baz"] == 0);
	REQUIRE(fake_filesystem["quux"] == 100'000);
	REQUIRE(fake_filesystem.contains("foo") == false);
}

TEST_CASE("A value can be replaced with another. This also updates the file")
{
	FakeFilesystem<int> fake_filesystem = {
		{"bar", -5},
		{"baz", 0},
		{"foo", 3},
		{"quux", 100'000},
	};

	auto v = TestFileVector::load(TestBasePathTrait<int>(fake_filesystem), TestLoadTrait<int>(fake_filesystem));
	
	v.replace(1, -78);

	REQUIRE(v.files()[1].content == -78);
	REQUIRE(fake_filesystem["baz"] == -78);
}

TEST_CASE("Add takes a file name, which is then concatenated with the base path")
{
	FakeFilesystem<int> fake_filesystem = {};

	auto v = TestFileVector::load(TestBasePathTrait<int>(fake_filesystem, "base/"), TestLoadTrait<int>(fake_filesystem));

	v.add(10, "file1.txt");
	v.add(20, "other_file.txt");
	v.add(30, "important document.txt");

	std::span<aeh::File<int> const> const files = v.files();

	REQUIRE(files.size() == 3);

	REQUIRE(files[0].path == "base/file1.txt");
	REQUIRE(files[0].content == 10);
	REQUIRE(files[1].path == "base/other_file.txt");
	REQUIRE(files[1].content == 20);
	REQUIRE(files[2].path == "base/important document.txt");
	REQUIRE(files[2].content == 30);

	REQUIRE(fake_filesystem.size() == 3);

	REQUIRE(fake_filesystem["base/file1.txt"] == 10);
	REQUIRE(fake_filesystem["base/other_file.txt"] == 20);
	REQUIRE(fake_filesystem["base/important document.txt"] == 30);
}
