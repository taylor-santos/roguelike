//
// Created by taylor-santos on 5/7/2021 at 17:14.
//

#include "sample.h"
#include "doctest/doctest.h"

int
fact(int n) {
    return n <= 1 ? 1 : fact(n - 1) * n;
}

TEST_CASE("fact") {
    CHECK(fact(0) == 1);
    CHECK(fact(1) == 1);
    CHECK(fact(2) == 2);
    CHECK(fact(3) == 6);
    CHECK(fact(10) == 3628800);
}
