//
// Created by taylor-santos on 12/18/2021 at 17:49.
//

#pragma once

#include <memory>
#include <functional>
#include <filesystem>

class Plugin {
public:
    class Function {
    public:
        using Signature = int (*)(void *);

        /***
         * Construct a Function object from a pointer to a function pointer.
         * @param fptr a pointer to a function pointer of type Signature.
         * @throws std::invalid_argument if fptr is NULL
         */
        explicit Function(Signature *fptr);

        /***
         * Invoke the function with the given argument, if it exists.
         * @param arg The argument to be passed to the function.
         * @return The returned value of the function if it exists, -1 if it doesn't.
         */
        int
        operator()(void *arg) const;

        [[nodiscard]] bool
        is_valid() const;

    private:
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

    // Get the name of the plugin.
    [[nodiscard]] std::string
    get_name() const;

    /***
     * Given the symbol name of a function, attempts to load it from the shared library.
     * @param name The name of the function.
     * @return if successful, a pointer to the function. Otherwise, nullptr.
     */
    [[nodiscard]] Function
    get_function(const std::string &name);

    bool
    reload_if_updated(int timeout_ms = 1000, int sleep_ms = 100);

    // Converts a library name, e.g. "my_lib", into a platform-specific filename.
    // On Windows, this would be "my_lib.dll". On Linux, "libmy_lib.so".
    static std::string
    shared_lib_name(const std::string &name) noexcept;

private:
    class Library {
    public:
        explicit Library(const std::filesystem::path &dir);
        Library(Library &&other) noexcept;
        Library &
        operator=(Library &&other) noexcept;
        ~Library();

        [[nodiscard]] Function
        get_function(const std::string &name);

        bool
        unload();

        bool
        load(const std::filesystem::path &dir);

    private:
        void *lib_ = nullptr;

        std::unordered_map<std::string, std::unique_ptr<Function::Signature>> funcs_;
    };

    std::string                     lib_name_;
    std::filesystem::path           lib_dir_;
    std::filesystem::file_time_type lib_time_;
    std::filesystem::path           tmp_dir_;
    Library                         lib_;
};
