//
// Created by taylor-santos on 12/13/2021 at 12:08.
//

#include "engine.h"

int
start(void *) {
    Console::log("---- my_plugin start ----");
    return 0;
}

int
update(void *) {
    Console::log("Hello, world");
    return 0;
}
