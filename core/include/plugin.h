//
// Created by taylor-santos on 12/18/2021 at 17:49.
//

#pragma once

#include <memory>
#include <functional>
#include <filesystem>
#include <unordered_map>

/***
 * Class for loading a C++ shared library from disk and retrieving functions from the library by
 * name. A method, reload_if_updated(), checks whether the library has since been updated and
 * reloads it if necessary. The function wrapper objects returned by the get_function(name) method
 * are automatically kept up-to-date when the library is reloaded.
 */
class Plugin {
public:
    /***
     * Wrapper for a function pointer returned from a shared library. Underlying function pointer
     * can be updated if the library is reloaded without needing to modify each Function wrapper
     * instance. The operator() passes its argument onto the wrapped function and returns its
     * result.
     * May wrap a nullptr, representing a non-existent function. This may come from requesting a
     * function by name from a library that does not implement it, or from reloading a library into
     * a version that no longer implements a function that used to be valid.
     */
    class Function {
    public:
        using Signature = int (*)(void *);

        /***
         * Construct a Function object from a pointer to a function pointer.
         * @param fptr a pointer to a function pointer of type Signature. May be a nullptr.
         */
        explicit Function(Signature *fptr);

        /***
         * Invoke the function with the given argument, if it exists. Does nothing and returns -1 if
         * the wrapped function is a nullptr.
         * @param arg The argument to be passed to the function.
         * @return The returned value of the function if it exists, -1 if it's a nullptr.
         */
        int
        operator()(void *arg) const;

        /***
         * Returns true when the wrapping a valid, usable function pointer. May become false if the
         * function's library is reloaded and no longer contains an implementation of this function.
         * Calling an invalid Function object has no effect and returns -1.
         */
        [[nodiscard]] bool
        is_valid() const;

    private:
        // Pointer to function pointer. Underlying function pointer can be updated when the library
        // is reloaded without needing to update this Function object.
        const Signature *fptr_;
    };

    /***
     * Load a shared library plugin from disk.
     * @param name The filename of the shared library, without extension or prefix.
     *             E.g. loading "libmy_plugin.so" would have the name "my_plugin".
     * @param directory The directory to load the plugin from.
     * @throws std::runtime_error if given an invalid file, if the library fails to load, or if
     *         other I/O errors occur.
     */
    Plugin(const std::string &name, const std::filesystem::path &directory);
    Plugin(Plugin &&other) noexcept;
    Plugin &
    operator=(Plugin &&other) noexcept;

    Plugin(const Plugin &) = delete;
    Plugin &
    operator=(const Plugin &) = delete;

    ~Plugin();

    /***
     * The plugin name refers to the filename of the shared library, without platform-specific
     * extensions or prefixes.
     */
    [[nodiscard]] std::string
    get_name() const;

    /***
     * Given the symbol name of a function, attempts to load it from the shared library.
     * @param name The name of the function.
     * @return a Function wrapper around the library function. If one could not be found, the
     *         returned Function object's is_valid() method will return false and calling it will
     *         have no effect and will return -1.
     */
    [[nodiscard]] Function
    get_function(const std::string &name);

    /***
     * Check whether or not the original shared library file on disk has been modified since last
     * checked. If it has, load it again and update all previously retrieved Function objects.
     * @param timeout_ms The maximum amount of time in milliseconds to wait for the file to become
     *                   available for reading.
     * @param sleep_ms If the file is temporarily unavailable, sleep for this number of milliseconds
     *                 before trying again.
     * @return false if either the library is unchanged since the last check, or if it fails to be
     *         loaded. An error message will be printed to std::cerr in the case of failure. Returns
     *         true if the library has been changed and is successfully reloaded.
     * @throws std::runtime_error if an I/O error occurs while reading/writing the new library, or
     *         if the old library fails to get unloaded.
     */
    bool
    reload_if_updated(int timeout_ms = 1000, int sleep_ms = 100);

    /***
     * Convert a library name into a platform-specific filename.
     * On Windows, "my_lib" becomes "my_lib.dll". On Linux, "libmy_lib.so".
     */
    static std::string
    shared_lib_name(const std::string &name) noexcept;

private:
    std::string                     lib_name_;
    std::filesystem::path           lib_dir_;
    std::filesystem::file_time_type lib_time_;
    std::filesystem::path           tmp_dir_;
    void                           *lib_ = nullptr;

    // Map function names to their pointers. When a library is updated, function pointers can be
    // updated without needing to change each individual Function wrapper object.
    std::unordered_map<std::string, std::unique_ptr<Function::Signature>> funcs_;

private:
    bool
    load_lib(const std::filesystem::path &dir);

    bool
    unload_lib();

    friend void
    swap(Plugin &first, Plugin &second) noexcept;
};
