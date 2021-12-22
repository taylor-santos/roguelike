//
// Created by taylor-santos on 12/20/2021 at 20:41.
//

#include "engine.h"

extern "C" DLL_PUBLIC int
start(void *ptr) {
    int *i = static_cast<int *>(ptr);
    *i     = 2;
    return 0;
}
