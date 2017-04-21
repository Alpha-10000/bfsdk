//
// Bareflank Hypervisor
//
// Copyright (C) 2015 Assured Information Security, Inc.
// Author: Rian Quinn        <quinnr@ainfosec.com>
// Author: Brendan Kerrigan  <kerriganb@ainfosec.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include <catch/catch.hpp>

#include <bffile.h>
#include <bfstring.h>
#include <hippomocks.h>

#include <memory>

TEST_CASE("read with bad filename")
{
    auto &&f = file{};
    auto &&filename = "/blah/bad_filename.txt"_s;

    CHECK_THROWS(f.read_text(""));
    CHECK_THROWS(f.read_binary(""));

    CHECK_THROWS(f.read_text(filename));
    CHECK_THROWS(f.read_binary(filename));
}

TEST_CASE("write with bad filename")
{
    auto &&f = file{};
    auto &&filename = "/blah/bad_filename.txt"_s;

    auto &&text_data = "hello"_s;
    auto &&binary_data = {'h', 'e', 'l', 'l', 'o'};

    CHECK_THROWS(f.write_text("", text_data));
    CHECK_THROWS(f.write_binary("", binary_data));

    CHECK_THROWS(f.write_text(filename, text_data));
    CHECK_THROWS(f.write_binary(filename, binary_data));
}

TEST_CASE("read / write success")
{
    auto &&f = file{};
    auto &&filename = "test.txt"_s;

    auto &&text_data1 = file::text_data{};
    auto &&text_data2 = "hello"_s;
    auto &&binary_data1 = file::binary_data{};
    auto &&binary_data2 = {'h', 'e', 'l', 'l', 'o'};

    REQUIRE_NOTHROW(f.write_text(filename, text_data1));
    CHECK(f.read_text(filename) == text_data1);

    REQUIRE_NOTHROW(f.write_binary(filename, binary_data1));
    CHECK(f.read_binary(filename) == binary_data1);

    REQUIRE_NOTHROW(f.write_text(filename, text_data2));
    CHECK(f.read_text(filename) == text_data2);

    REQUIRE_NOTHROW(f.write_binary(filename, binary_data2));
    CHECK(f.read_binary(filename) == file::binary_data(binary_data2));

    REQUIRE(std::remove(filename.c_str()) == 0);
}

TEST_CASE("extension")
{
    auto &&f = file{};

    CHECK(f.extension("") == "");
    CHECK(f.extension("no_extension") == "");
    CHECK(f.extension("no_extension.") == ".");
    CHECK(f.extension(".nofilename") == ".nofilename");
    CHECK(f.extension("no_path.ext") == ".ext");
    CHECK(f.extension("/with/path.ext") == ".ext");
    CHECK(f.extension("more.than.one.ext") == ".ext");
    CHECK(f.extension(R"(c:\windows\path.ext)") == ".ext");
}

TEST_CASE("exists")
{
    auto &&f = file{};
    auto &&filename = "test.txt"_s;

    CHECK(!f.exists(""));
    CHECK(!f.exists(filename));

    REQUIRE_NOTHROW(f.write_text(filename, "hello world"));
    CHECK(f.exists(filename));

    REQUIRE(std::remove(filename.c_str()) == 0);

    CHECK(!f.exists(filename));
}

TEST_CASE("find files")
{
    auto &&f = file{};
    auto &&files = {"test1.txt"_s, "test2.txt"_s};
    auto &&paths = {"../bad/path"_s, "."_s};

    CHECK_THROWS(f.find_files({}, paths));
    CHECK_THROWS(f.find_files(files, {}));

    CHECK_THROWS(f.find_files(files, {"../file_not_found"_s}));

    for (const auto &file : files) {
        REQUIRE_NOTHROW(f.write_text(file, "hello world"));
    }

    auto &&results = f.find_files(files, paths);

    REQUIRE(results.size() == files.size());
    CHECK(results.at(0) == "./test1.txt");
    CHECK(results.at(1) == "./test2.txt");

    for (const auto &file : files) {
        REQUIRE(std::remove(file.c_str()) == 0);
    }
}

TEST_CASE("file size")
{
    auto &&f = file{};
    auto &&filename = "test.txt"_s;

    auto &&text_data = "hello"_s;
    auto &&binary_data = {'h', 'e', 'l', 'l', 'o'};

    CHECK_THROWS(f.size(""));
    CHECK_THROWS(f.size("bad_filename"));

    REQUIRE_NOTHROW(f.write_text(filename, text_data));
    CHECK(f.size(filename) == 5);

    REQUIRE_NOTHROW(f.write_binary(filename, binary_data));
    CHECK(f.size(filename) == 5);

    REQUIRE(std::remove(filename.c_str()) == 0);
}

#ifdef _HIPPOMOCKS__ENABLE_CFUNC_MOCKING_SUPPORT

TEST_CASE("home")
{
    auto &&f = file{};
    auto &&home = std::make_unique<char[]>(256);
    auto &&homepath = std::make_unique<char[]>(256);

    std::strncpy(home.get(), "home", 256);
    std::strncpy(homepath.get(), "homepath", 256);

    MockRepository mocks;

    mocks.OnCallFunc(std::getenv).Do([&](auto var) {
        if (std::string(var) == "HOME") {
            return static_cast<char *>(nullptr);
        }
        return homepath.get();
    });
    CHECK(f.home() == "homepath");

    mocks.OnCallFunc(std::getenv).Do([&](auto) {
        return home.get();
    });
    CHECK(f.home() == "home");

    mocks.OnCallFunc(std::getenv).Do([&](auto) {
        return static_cast<char *>(nullptr);
    });
    CHECK_THROWS(f.home());
}

#endif
