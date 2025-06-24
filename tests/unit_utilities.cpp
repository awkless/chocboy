// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include "cocoa/utilities.hpp"

#include <cstdint>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("void set_bit<T, Position>(T& data)", "[set_bit]")
{
    uint8_t data = 0x00;
    cocoa::set_bit<uint8_t, 0>(data);
    cocoa::set_bit<uint8_t, 4>(data);
    cocoa::set_bit<uint8_t, 7>(data);
    REQUIRE(data == 0b10010001);
}
