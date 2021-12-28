//
// Created by taylor-santos on 5/19/2021 at 18:17.
//

#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <filesystem>

#include "glm/glm.hpp"

namespace Util {

template<typename time_point_t>
auto
put_time_point(const time_point_t &tp, const std::string &fmt = "%F %T") {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(
        tp - time_point_t::clock::now() + system_clock::now());
    auto tt = system_clock::to_time_t(sctp);
    auto tm = std::localtime(&tt);
    return std::put_time(tm, fmt.c_str());
}

} // namespace Util

namespace Debug {

template<typename... Args>
void
log(const char *file, int line, const char *func, Args &&...args) {
    using namespace std::chrono;
    auto path    = relative(std::filesystem::path(file));
    auto message = std::stringstream();

    message << "[" << Util::put_time_point(system_clock::now()) << " " << path.string() << ":"
            << line << " " << func << "] ";
    (message << ... << args);
    std::cout << message.str() << std::endl;
}

template<typename... Args>
void
err(const char *file, int line, const char *func, Args &&...args) {
    using namespace std::chrono;
    auto now     = system_clock::now();
    auto tt      = system_clock::to_time_t(now);
    auto tm      = std::localtime(&tt);
    auto path    = std::filesystem::path(file);
    auto message = std::stringstream();

    message << "[" << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << " " << path.filename().string()
            << ":" << line << " " << func << "] ";
    (message << ... << args);
    std::cerr << message.str() << std::endl;
}

#define err(...) log(__FILE__, __LINE__, __func__, "ERROR: ", __VA_ARGS__)
#define log(...) log(__FILE__, __LINE__, __func__, "DEBUG: ", __VA_ARGS__)

} // namespace Debug

namespace glm {

template<int N, typename T>
inline std::ostream &
operator<<(std::ostream &os, const glm::vec<N, T> &vec) {
    // os << "{";
    std::string sep;
    for (int i = 0; i < N; i++) {
        os << std::fixed << sep << vec[i] << "f";
        sep = ",";
    }
    // os << "}";
    return os;
}

template<int N, typename T, glm::qualifier Q>
inline std::ostream &
operator<<(std::ostream &os, const glm::mat<N, N, T, Q> &mat) {
    std::string sep = "{{";
    for (int row = 0; row < N; row++) {
        os << std::fixed << sep << mat[row];
        sep = "},{";
    }
    os << "}}";
    return os;
}

template<typename T, glm::qualifier Q>
std::ostream &
operator<<(std::ostream &os, const glm::qua<T, Q> &q) {
    os << std::fixed << q.w << "f," << q.x << "f," << q.y << "f," << q.z << "f";
    return os;
}

} // namespace glm
