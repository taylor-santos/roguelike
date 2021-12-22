//
// Created by taylor-santos on 12/13/2021 at 12:08.
//

#include "engine.h"

static int
my_plugin() {
    Console::log("Hello, world");
    return 0;
}

extern "C" DLL_PUBLIC int
start(void *state) {
    return 0;
}

extern "C" DLL_PUBLIC int
update(void *state) {
    return my_plugin();
}
