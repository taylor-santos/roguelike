//
// Created by taylor-santos on 12/13/2021 at 12:08.
//

#include "engine.h"

#include <stdexcept>

static void
my_plugin() {
    Console::log("Hello, world");
}

extern "C" DLL_PUBLIC void
start(void *state) {}

extern "C" DLL_PUBLIC void
update(void *state) {
    my_plugin();
}
