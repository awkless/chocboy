// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "cocoa/utilities.hpp"

TEST_CASE("constexpr void cocoa::set_bit(T&)", "[set_bit]")
{
    uint8_t data = 0x00;
    cocoa::set_bit<uint8_t, 0>(data);
    cocoa::set_bit<uint8_t, 4>(data);
    cocoa::set_bit<uint8_t, 7>(data);
    REQUIRE(data == 0b10010001);
}

TEST_CASE("constexpr void cocoa::clear_bit(T&)", "[clear_bit]")
{
    uint8_t data = 0xFF;
    cocoa::clear_bit<uint8_t, 0>(data);
    cocoa::clear_bit<uint8_t, 4>(data);
    cocoa::clear_bit<uint8_t, 7>(data);
    REQUIRE(data == 0b01101110);
}

TEST_CASE("constexpr void cocoa::conditional_bit_toggle(T&)", "[conditional_bit_toggle]")
{
    uint8_t data = 0x20;
    cocoa::conditional_bit_toggle<uint8_t, 0>(data, true);
    cocoa::conditional_bit_toggle<uint8_t, 5>(data, false);
    REQUIRE(data == 0b00000001);
}
