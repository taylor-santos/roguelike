#ifndef ENGINE_H
#define ENGINE_H

#include <string>

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#    define PLUGIN __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#    define PLUGIN __declspec(dllexport)
#else
#    define PLUGIN extern "C"
#endif

extern "C" PLUGIN int
start(void *);

extern "C" PLUGIN int
update(void *);

namespace Console {

PLUGIN
void
log(const std::string &str, const char *file, int line, const char *func);

#define log(STR) log(STR, __FILE__, __LINE__, __func__)

} // namespace Console

#endif // ENGINE_H
