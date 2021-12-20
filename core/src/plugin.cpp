//
// Created by taylor-santos on 12/18/2021 at 17:51.
//

#include "plugin.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <thread>
#include <sstream>

#include "util.h"

// Converts a library name, e.g. "my_lib", into a platform-specific filename.
// On Windows, this would be "my_lib.dll". On Linux, "libmy_lib.so".
static std::string
shared_lib_name(const std::string &name) noexcept;

// Returns a human-readable string detailing the most recent library error.
static std::string
get_error_str() noexcept;

// Load the shared library located at dir, and return a handle to it that can be used with other
// library functions. Returns nullptr if any failure occurs, the details of which can be retrieved
// from get_error_str().
static void *
load_library(const char *dir) noexcept;

// Unload the library pointed to by the lib handle. If this is unsuccessful, returns true and the
// details can be retrieved from get_error_str(). Otherwise returns false.
static bool
unload_library(void *lib) noexcept;

// Given a library handle and a function name, returns the address of the associated function
// pointer. If this is unsuccessful, returns nullptr and the details can be retrieved from
// get_error_str().
static void *
get_library_function(void *lib, const char *func_name) noexcept;

// Creates a new temporary file sharing the same filename as the path argument. Then copies the
// contents of the file located at path into the new temporary file. Returns the path to this new
// temporary file.
// Throws an std::runtime_error if any I/O errors occur.
static std::filesystem::path
copy_file_to_temp(const std::filesystem::path &path);

Plugin::Plugin(const std::string &name, const std::filesystem::path &directory)
    : lib_name_(name)
    , lib_dir_(directory / shared_lib_name(name))
    , lib_time_(last_write_time(lib_dir_))
    , tmp_dir_(copy_file_to_temp(lib_dir_))
    , lib_(tmp_dir_) {
    Debug::log(
        "loaded plugin ",
        lib_name_,
        " from ",
        lib_dir_.string(),
        " into ",
        tmp_dir_.string());
}

std::string
Plugin::get_name() const {
    return lib_name_;
}

Plugin::FuncType
Plugin::get_function(const std::string &name) const {
    return lib_.get_function(name);
}

bool
Plugin::check_for_updates(int timeout_ms, int sleep_ms) {
    auto ec   = std::error_code();
    auto time = last_write_time(lib_dir_, ec);
    if (!ec && time > lib_time_) {
        Debug::log(lib_name_, " has an update, trying to read it...");
        {
            using namespace std::chrono;
            auto start = steady_clock::now();
            auto src   = std::ifstream();
            Debug::log("attempting to open new library");
            while (true) {
                // In addition to checking if the file is readable, we need to check that it has
                // been written to. Passing the flag std::ios::ate to src.open() seeks to the end
                // of the file, and src.tellg() returns the current position. If the file is empty,
                // this will be 0.
                src.open(lib_dir_, std::ios::in | std::ios::binary | std::ios::ate);
                if (src) {                 // File is open...
                    if (src.tellg() > 0) { // File is non-empty...
                        src.seekg(0);      // Return to beginning of file.
                        break;
                    }
                    Debug::err("file is readable, but empty");
                    src.close(); // Close the file so that it can be written to while we sleep.
                }
                if (steady_clock::now() - start > milliseconds(timeout_ms)) {
                    Debug::err("failed to load ", lib_dir_.string(), " after ", timeout_ms, "ms");
                    return false;
                }
                Debug::log("sleeping for ", sleep_ms, "ms...");
                std::this_thread::sleep_for(milliseconds(sleep_ms));
            }
            Debug::log("successfully opened new library");
            if (lib_.unload()) {
                throw std::runtime_error(get_error_str());
            }
            auto dst = std::ofstream();
            Debug::log("attempting to open old library");
            while (true) {
                dst.open(tmp_dir_, std::ios::out | std::ios::binary | std::ios::trunc);
                if (dst) break;
                if (steady_clock::now() - start > milliseconds(timeout_ms)) {
                    std::stringstream ss;
                    ss << "failed to load " << lib_dir_.string() << " after " << timeout_ms
                       << "ms, previous version is now invalid";
                    throw std::runtime_error(ss.str());
                }
                Debug::log("sleeping for ", sleep_ms, "ms...");
                std::this_thread::sleep_for(milliseconds(sleep_ms));
            }
            Debug::log("successfully opened old library");
            dst << src.rdbuf();
        }
        Debug::log("attempting to load new library");
        if (lib_.load(tmp_dir_)) {
            Debug::err(get_error_str());
            return false;
        }
        Debug::log("successfully loaded new library");
        lib_time_ = last_write_time(lib_dir_, ec);
        return true;
    }
    return false;
}

static std::filesystem::path
copy_file_to_temp(const std::filesystem::path &path) {
    auto temp_path = std::filesystem::temp_directory_path() / path.filename();

    std::ifstream src(path, std::ios::in | std::ios::binary);
    if (!src) {
        throw std::runtime_error(path.string() + ": " + std::strerror(errno));
    }

    std::ofstream dst(temp_path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!dst) {
        throw std::runtime_error(temp_path.string() + ": " + std::strerror(errno));
    }

    dst << src.rdbuf();
    return temp_path;
}

Plugin::Library::Library(const std::filesystem::path &dir)
    : lib_(load_library(dir.string().c_str())) {
    if (!lib_) {
        throw std::runtime_error(get_error_str());
    }
}

Plugin::Library::~Library() {
    if (lib_ && unload_library(lib_)) {
        Debug::err(get_error_str());
    }
}

Plugin::FuncType
Plugin::Library::get_function(const std::string &name) const {
    if (!lib_) return nullptr;
    auto func = get_library_function(lib_, name.c_str());
    if (!func) {
        throw std::runtime_error(get_error_str());
    }
    return (Plugin::FuncType)func;
}

bool
Plugin::Library::unload() {
    if (lib_ && unload_library(lib_)) {
        return true;
    }
    lib_ = nullptr;
    return false;
}

bool
Plugin::Library::load(const std::filesystem::path &dir) {
    if (lib_ && unload()) {
        return true;
    }
    lib_ = load_library(dir.string().c_str());

    return !lib_;
}

Plugin::Library::Library(Plugin::Library &&other) noexcept {
    std::swap(this->lib_, other.lib_);
}

Plugin::Library &
Plugin::Library::operator=(Plugin::Library &&other) noexcept {
    std::swap(this->lib_, other.lib_);
    return *this;
}

//------------------------------------------------------------------------------------
// Platform-Specific Implementations
//------------------------------------------------------------------------------------

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
// Linux

#    include <dlfcn.h>

inline void *
load_library(const char *dir) noexcept {
    return dlopen(dir, RTLD_LAZY);
}

inline bool
unload_library(void *lib) noexcept {
    return dlclose(lib);
}

inline void *
get_library_function(void *lib, const char *func_name) noexcept {
    return dlsym(lib, func_name);
}

std::string
shared_lib_name(const std::string &name) noexcept {
    return "lib" + name + ".so";
}

std::string
get_error_str() noexcept {
    return dlerror();
}

#elif defined(_MSC_VER)
// Windows

#    include <windows.h>

inline void *
load_library(const char *dir) noexcept {
    return (void *)LoadLibraryA(dir);
}

inline bool
unload_library(void *lib) noexcept {
    return !FreeLibrary((HMODULE)lib); // FreeLibrary returns nonzero on success.
}

inline void *
get_library_function(void *lib, const char *func_name) noexcept {
    return GetProcAddress((HMODULE)lib, func_name);
}

std::string
shared_lib_name(const std::string &name) noexcept {
    return name + ".dll";
}

std::string
get_error_str() noexcept {
    auto  id  = GetLastError();
    LPSTR buf = nullptr;
    auto  sz  = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        id,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&buf,
        0,
        NULL);
    std::string error(buf, sz);
    LocalFree(buf);
    return error;
}

#else
#    error "Unsupported platform"
#endif
