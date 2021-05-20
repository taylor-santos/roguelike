//
// Created by taylor-santos on 5/19/2021 at 18:17.
//

#ifndef ROGUELIKE_INCLUDE_UTIL_H
#define ROGUELIKE_INCLUDE_UTIL_H

namespace glm {
template<int N, typename T>
std::ostream &
operator<<(std::ostream &os, const glm::vec<N, T> &vec) {
    os << "{";
    std::string sep;
    for (int i = 0; i < N; i++) {
        os << std::fixed << sep << vec[i];
        sep = ",";
    }
    os << "}";
    return os;
}

template<int N, typename T>
std::ostream &
operator<<(std::ostream &os, const glm::mat<N, N, T, glm::defaultp> &mat) {
    std::string sep = "{";
    for (int row = 0; row < N; row++) {
        os << std::fixed << sep << mat[row];
        sep = ",\n";
    }
    os << "}";
    return os;
}

std::ostream &
operator<<(std::ostream &os, const glm::quat &q) {
    os << std::fixed << q.w << "," << q.x << "," << q.y << "," << q.z;
    return os;
}
} // namespace glm

#endif // ROGUELIKE_INCLUDE_UTIL_H
