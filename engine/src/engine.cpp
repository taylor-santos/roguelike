//
// Created by taylor-santos on 12/13/2021 at 16:30.
//

#include "engine.h"
#undef log

#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace Console {

DLL_PUBLIC
void
log(const std::string &str, const char *file, int line, const char *func) {
    using namespace std::chrono;
    auto now     = system_clock::now();
    auto tt      = system_clock::to_time_t(now);
    auto tm      = std::localtime(&tt);
    auto path    = std::filesystem::path(file);
    auto message = std::stringstream();

    message << "[" << std::put_time(tm, "%F %T") << " " << path.filename().string() << ":" << line
            << " " << func << "] " << str;
    std::cout << message.str() << std::endl;
}

} // namespace Console
