//
// Created by taylor-santos on 12/18/2021 at 17:51.
//

#include "plugin.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <thread>
#include <utility>

#include "util.h"

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

static std::filesystem::path
temp_path();

static std::filesystem::path
copy_file_to_temp(std::ifstream &&src);

Plugin::Function::Function(Signature *fptr)
    : fptr_{fptr} {
    if (!fptr) throw std::invalid_argument("Function constructor given nullptr");
}

int
Plugin::Function::operator()(void *arg) const {
    auto fptr = *fptr_;
    return fptr ? fptr(arg) : -1;
}

bool
Plugin::Function::is_valid() const {
    return *fptr_;
}

Plugin::Plugin(const std::string &name, const std::filesystem::path &directory)
    : lib_name_(name)
    , lib_dir_(directory / shared_lib_name(name))
    , lib_time_(last_write_time(lib_dir_))
    , tmp_dir_(copy_file_to_temp(std::ifstream(lib_dir_, std::ios::binary)))
    , lib_(tmp_dir_) {
    Debug::log(
        "loaded plugin ",
        lib_name_,
        " from ",
        lib_dir_.string(),
        " into ",
        tmp_dir_.string());
    Debug::log(lib_dir_.string(), " has write time ", Util::put_time_point(lib_time_));
}

std::string
Plugin::get_name() const {
    return lib_name_;
}

Plugin::Function
Plugin::get_function(const std::string &name) {
    return lib_.get_function(name);
}

bool
Plugin::reload_if_updated(int timeout_ms, int sleep_ms) {
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
                std::string error = get_error_str();
                Debug::log("failed to unload old library: ", error);
                throw std::runtime_error(error);
            }
            Debug::log("creating new temporary file for library");
            tmp_dir_ = copy_file_to_temp(std::move(src));
        }
        Debug::log("attempting to load new library");
        if (lib_.load(tmp_dir_)) {
            std::string error = get_error_str();
            Debug::log("failed to load new library: ", error);
            Debug::err(error);
            return false;
        }
        Debug::log("successfully loaded new library");
        lib_time_ = last_write_time(lib_dir_, ec);
        return true;
    }
    return false;
}

static std::filesystem::path
copy_file_to_temp(std::ifstream &&src) {
    if (!src.fail()) { // if file is valid (goodbit | eofbit), reset to beginning
        src.seekg(0);
    }
    // seekg might fail, check again
    if (src.fail()) {
        // throw file's failure as exception
        src.exceptions(std::fstream::failbit | std::fstream::badbit);
    }
    auto tmp = temp_path();

    std::ofstream dst(tmp, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!dst) {
        throw std::runtime_error(tmp.string() + ": " + std::strerror(errno));
    }

    dst << src.rdbuf();
    return tmp;
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

Plugin::Function
Plugin::Library::get_function(const std::string &name) {
    auto it = funcs_.find(name);
    if (it == funcs_.end()) {
        // Function has *not* been retrieved before, query the library
        auto fptr = lib_ ? (Function::Signature)get_library_function(lib_, name.c_str()) : nullptr;
        auto uptr = std::make_unique<Function::Signature>(fptr);
        auto ptr  = uptr.get();
        funcs_[name] = std::move(uptr);
        return Function(ptr);
    }
    // Function has been retrieved before and still has active users
    return Function(it->second.get());
}

bool
Plugin::Library::unload() {
    Debug::log("unloading library");
    for (auto &[name, fptr] : funcs_) {
        *fptr = nullptr;
    }
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

    for (auto &[name, fptr] : funcs_) {
        *fptr = lib_ ? (Function::Signature)get_library_function(lib_, name.c_str()) : nullptr;
    }

    return !lib_;
}

Plugin::Library::Library(Plugin::Library &&other) noexcept {
    std::swap(this->lib_, other.lib_);
    std::swap(this->funcs_, other.funcs_);
}

Plugin::Library &
Plugin::Library::operator=(Plugin::Library &&other) noexcept {
    std::swap(this->lib_, other.lib_);
    std::swap(this->funcs_, other.funcs_);
    return *this;
}

//------------------------------------------------------------------------------------
// Platform-Specific Implementations
//------------------------------------------------------------------------------------

#if defined(_MSC_VER) || defined(__MINGW32__)
// Windows

#    include <windows.h>

std::string
get_error_str() noexcept {
    auto  id  = GetLastError();
    LPSTR buf = nullptr;
    auto  sz  = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        id,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&buf,
        0,
        nullptr);
    std::string error(buf, sz);
    LocalFree(buf);
    return error;
}

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
    return (void *)GetProcAddress((HMODULE)lib, func_name);
}

inline std::filesystem::path
temp_path() {
    TCHAR path_buf[MAX_PATH];
    TCHAR file_buf[MAX_PATH];
    DWORD p = GetTempPath(MAX_PATH, path_buf);
    if (p > MAX_PATH || p == 0) {
        throw std::runtime_error(get_error_str());
    }
    UINT f = GetTempFileName(path_buf, 0, 0, file_buf);
    if (f == 0) {
        throw std::runtime_error(get_error_str());
    }
    return {file_buf};
}

std::string
Plugin::shared_lib_name(const std::string &name) noexcept {
#    if defined(_MSC_VER)
    return name + ".dll";
#    elif defined(__MINGW32__)
    return "lib" + name + ".dll";
#    endif
}

#elif defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
// Linux/MacOS

#    include <dlfcn.h>

#    include <unistd.h>

std::string
get_error_str() noexcept {
    return dlerror();
}

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

static std::filesystem::path
temp_path() {
    // This a bit of a hacky workaround to tmpnam(3) being deprecated. The two alternative
    // functions, mkstemp(3) and tmpfile(3) return a file descriptor and FILE* respectively. But we
    // need just a filename because it is going to be stored and later opened as an std::fstream. So
    // we can use mkstemp(3) to create a file and write its path, then immediately close the file
    // and return the path. This obviously is not an ideal solution, so TODO: find a better way to
    // do this.
    auto path     = std::filesystem::temp_directory_path();
    auto path_str = (path / "XXXXXX").string();
    auto buf      = std::make_unique<char[]>(path_str.length() + 1);
    memcpy(buf.get(), path_str.c_str(), path_str.size() + 1);
    int fd = mkstemp(buf.get());
    close(fd);
    return buf.get();
}

std::string
Plugin::shared_lib_name(const std::string &name) noexcept {
#    if defined(__APPLE__)
    return "lib" + name + ".dylib";
#    elif defined(__CYGWIN__)
    return "cyg" + name + ".dll";
#    else
    return "lib" + name + ".so";
#    endif
}

#else
#    error "Unsupported platform"
#endif
