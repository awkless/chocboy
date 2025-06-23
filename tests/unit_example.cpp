// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Example", "[example]")
{
    int a = 0x61;
    REQUIRE(static_cast<char>(a) == 'a');
}
