//
// Created by taylor-santos on 12/18/2021 at 17:49.
//

#pragma once

#include <memory>
#include <functional>
#include <filesystem>

class Plugin {
public:
    using FuncType = void (*)(void *);

    explicit Plugin(const std::string &name, const std::filesystem::path &directory);

    [[nodiscard]] std::string
    get_name() const;

    [[nodiscard]] Plugin::FuncType
    get_function(const std::string &name) const;

    bool
    check_for_updates(int timeout_ms = 1000, int sleep_ms = 100);

private:
    class Library {
    public:
        explicit Library(const std::filesystem::path &dir);
        Library(Library &&other) noexcept;
        Library &
        operator=(Library &&other) noexcept;
        ~Library();

        [[nodiscard]] Plugin::FuncType
        get_function(const std::string &name) const;

        bool
        unload();

        bool
        load(const std::filesystem::path &dir);

    private:
        void *lib_ = nullptr;
    };

    std::string                     lib_name_;
    std::filesystem::path           lib_dir_;
    std::filesystem::file_time_type lib_time_;
    std::filesystem::path           tmp_dir_;
    Library                         lib_;
};
