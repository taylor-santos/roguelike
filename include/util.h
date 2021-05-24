//
// Created by taylor-santos on 5/19/2021 at 18:17.
//

#ifndef ROGUELIKE_INCLUDE_UTIL_H
#define ROGUELIKE_INCLUDE_UTIL_H

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

#endif // ROGUELIKE_INCLUDE_UTIL_H
