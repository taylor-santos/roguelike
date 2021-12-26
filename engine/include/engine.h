#ifndef ENGINE_H
#define ENGINE_H

#include <string>

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#    define PLUGIN extern "C" __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#    define PLUGIN extern "C" __declspec(dllexport)
#else
#    define PLUGIN extern "C"
#endif

#if defined _WIN32 || defined __CYGWIN__
#    ifdef __GNUC__
#        define DLL_PUBLIC __attribute__((dllexport))
#    else
#        define DLL_PUBLIC \
            __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#    endif
#else
#    if __GNUC__ >= 4
#        define DLL_PUBLIC __attribute__((visibility("default")))
#    else
#        define DLL_PUBLIC
#    endif
#endif

extern "C" DLL_PUBLIC int
start(void *);

extern "C" DLL_PUBLIC int
update(void *);

namespace Console {

DLL_PUBLIC
void
log(const std::string &str, const char *file, int line, const char *func);

#define log(STR) log(STR, __FILE__, __LINE__, __func__)

} // namespace Console

#endif // ENGINE_H
