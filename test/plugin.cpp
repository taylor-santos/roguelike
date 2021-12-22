//
// Created by taylor-santos on 12/20/2021 at 16:32.
//

#include "doctest/doctest.h"

#include "plugin.h"

#include <fstream>

TEST_SUITE_BEGIN("Plugin");

TEST_CASE("InvalidPluginDirectory") {
    auto path = "foo/bar";
    CHECK_THROWS_AS_MESSAGE(
        Plugin("plugin", path),
        std::runtime_error,
        "path doesn't exist so it should throw");
}

TEST_CASE("InvalidPluginName") {
    auto path = std::filesystem::current_path() / "plugins";
    CHECK_THROWS_AS_MESSAGE(
        Plugin("fake_plugin", path),
        std::runtime_error,
        "plugin doesn't exist so it should throw");
}

TEST_CASE("PluginLoad") {
    auto path = std::filesystem::current_path() / "plugins";
    CHECK_NOTHROW_MESSAGE(
        Plugin("plugin", path),
        "plugin is valid so it should construct without throwing");
}

TEST_CASE("PluginName") {
    auto path   = std::filesystem::current_path() / "plugins";
    auto plugin = Plugin("plugin", path);
    CHECK(plugin.get_name() == "plugin");
}

TEST_CASE("PluginFunction") {
    auto   path = std::filesystem::current_path() / "plugins";
    Plugin plugin("plugin", path);
    auto   start = plugin.get_function("start");
    int    val   = 0;
    CHECK_MESSAGE(start(&val) == 0, "plugin function should exist and should return 0");
    CHECK_MESSAGE(val == 1, "plugin function should modify argument to point to value 1");
}

TEST_CASE("PluginReloading") {
    auto tmp_name = Plugin::shared_lib_name("test_plugin");
    auto path     = std::filesystem::current_path() / "plugins";
    auto tmp_path = std::filesystem::temp_directory_path();
    {
        auto lib_name = Plugin::shared_lib_name("plugin");
        auto src      = std::ifstream(path / lib_name, std::ios::binary);
        REQUIRE_MESSAGE(src, "couldn't open plugin file");
        auto dst = std::ofstream(tmp_path / tmp_name, std::ios::binary | std::ios::trunc);
        REQUIRE_MESSAGE(dst, "couldn't open temporary output file");
        dst << src.rdbuf();
    }
    Plugin plugin("test_plugin", tmp_path);
    auto   start = plugin.get_function("start");
    int    val   = 0;
    CHECK_MESSAGE(start(&val) == 0, "plugin function should exist and should return 0");
    CHECK_MESSAGE(val == 1, "plugin function should modify argument to point to value 1");
    {
        auto lib_name = Plugin::shared_lib_name("plugin_patch");
        auto src      = std::ifstream(path / lib_name, std::ios::binary);
        REQUIRE_MESSAGE(src, "couldn't open plugin file");
        auto dst = std::ofstream(tmp_path / tmp_name, std::ios::binary | std::ios::trunc);
        REQUIRE_MESSAGE(dst, "couldn't open temporary output file");
        dst << src.rdbuf();
    }
    CHECK_MESSAGE(
        plugin.reload_if_updated(),
        "plugin library file has changed, it should detect and load an update");
    CHECK_MESSAGE(start(&val) == 0, "plugin function should exist and should return 0");
    CHECK_MESSAGE(val == 2, "updated plugin function should modify argument to point to value 2");
}

TEST_CASE("PluginRemovedFunction") {
    auto tmp_name = Plugin::shared_lib_name("test_plugin");
    auto path     = std::filesystem::current_path() / "plugins";
    auto tmp_path = std::filesystem::temp_directory_path();
    {
        auto lib_name = Plugin::shared_lib_name("plugin");
        auto src      = std::ifstream(path / lib_name, std::ios::binary);
        REQUIRE_MESSAGE(src, "couldn't open plugin file");
        auto dst = std::ofstream(tmp_path / tmp_name, std::ios::binary | std::ios::trunc);
        REQUIRE_MESSAGE(dst, "couldn't open temporary output file");
        dst << src.rdbuf();
    }
    Plugin plugin("test_plugin", tmp_path);
    auto   update = plugin.get_function("update");
    CHECK_MESSAGE(update(nullptr) == 1, "plugin function should exist and should return 1");
    {
        auto lib_name = Plugin::shared_lib_name("plugin_patch");
        auto src      = std::ifstream(path / lib_name, std::ios::binary);
        REQUIRE_MESSAGE(src, "couldn't open plugin file");
        auto dst = std::ofstream(tmp_path / tmp_name, std::ios::binary | std::ios::trunc);
        REQUIRE_MESSAGE(dst, "couldn't open temporary output file");
        dst << src.rdbuf();
    }
    CHECK_MESSAGE(
        plugin.reload_if_updated(),
        "plugin library file has changed, it should detect and load an update");
    CHECK_MESSAGE(
        update(nullptr) == -1,
        "plugin function should no longer exist, so it returns -1");
    CHECK_MESSAGE(update.is_valid() == false, "plugin function should now be invalidated");
}

TEST_CASE("PluginReloadingDeleted") {
    auto tmp_name = Plugin::shared_lib_name("test_plugin");
    auto path     = std::filesystem::current_path() / "plugins";
    auto tmp_path = std::filesystem::temp_directory_path();
    {
        auto lib_name = Plugin::shared_lib_name("plugin");
        auto src      = std::ifstream(path / lib_name, std::ios::binary);
        REQUIRE_MESSAGE(src, "couldn't open plugin file");
        auto dst = std::ofstream(tmp_path / tmp_name, std::ios::binary | std::ios::trunc);
        REQUIRE_MESSAGE(dst, "couldn't open temporary output file");
        dst << src.rdbuf();
    }
    Plugin plugin("test_plugin", tmp_path);
    {
        auto dst = std::ofstream(tmp_path / tmp_name, std::ios::binary | std::ios::trunc);
        REQUIRE_MESSAGE(dst, "couldn't open temporary output file");
    }
    CHECK_MESSAGE(
        plugin.reload_if_updated(100) == false,
        "original plugin is deleted, reloading should fail after a delay");
}

TEST_CASE("PluginMoveConstruct") {
    auto   path = std::filesystem::current_path() / "plugins";
    Plugin plugin("plugin", path);
    auto   start = plugin.get_function("start");
    int    val   = 0;
    CHECK_MESSAGE(start(&val) == 0, "plugin function should exist and should return 0");
    CHECK_MESSAGE(val == 1, "plugin function should modify argument to point to value 1");
    val         = 0;
    Plugin copy = std::move(plugin);
    CHECK_MESSAGE(copy.get_name() == "plugin", "new plugin has correct name");
    CHECK_MESSAGE(start.is_valid(), "function is still valid after move");
    CHECK_MESSAGE(start(&val) == 0, "function can still be invoked after move");
    CHECK_MESSAGE(val == 1, "function should still have the same effect");
    auto update = copy.get_function("update");
    CHECK_MESSAGE(update.is_valid(), "new plugin should be able to access new functions");
}

TEST_CASE("PluginMoveAssign") {
    auto   path = std::filesystem::current_path() / "plugins";
    Plugin plugin("plugin", path);
    auto   start = plugin.get_function("start");
    int    val   = 0;
    CHECK_MESSAGE(start(&val) == 0, "plugin function should exist and should return 0");
    CHECK_MESSAGE(val == 1, "plugin function should modify argument to point to value 1");
    val = 0;
    Plugin copy("plugin_patch", path);
    copy = std::move(plugin);
    CHECK_MESSAGE(copy.get_name() == "plugin", "new plugin has correct name");
    CHECK_MESSAGE(start.is_valid(), "function is still valid after move");
    CHECK_MESSAGE(start(&val) == 0, "function can still be invoked after move");
    CHECK_MESSAGE(val == 1, "function should still have the same effect");
    auto update = copy.get_function("update");
    CHECK_MESSAGE(update.is_valid(), "new plugin should be able to access new functions");
}
