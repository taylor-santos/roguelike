//
// Created by taylor-santos on 12/20/2021 at 20:41.
//

#include "engine.h"

int
start(void *ptr) {
    auto *i = static_cast<int *>(ptr);

    *i = 2;
    return 0;
}
