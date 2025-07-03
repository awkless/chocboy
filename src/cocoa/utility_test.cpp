// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "cocoa/utility.hpp"

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

TEST_CASE("constexpr bool cocoa::is_bit_set(T)", "[is_bit_set]")
{
    uint8_t data = 0b10001001;
    REQUIRE(cocoa::is_bit_set<uint8_t, 0>(data) == true);
    REQUIRE(cocoa::is_bit_set<uint8_t, 3>(data) == true);
    REQUIRE(cocoa::is_bit_set<uint8_t, 4>(data) == false);
    REQUIRE(cocoa::is_bit_set<uint8_t, 5>(data) == false);
    REQUIRE(cocoa::is_bit_set<uint8_t, 7>(data) == true);
}

TEST_CASE("constexpr void cocoa::complement_bit(T&)", "[complement_bit]")
{
    uint8_t data = 0b10001001;

    cocoa::toggle_bit<uint8_t, 0>(data);
    REQUIRE(cocoa::is_bit_set<uint8_t, 0>(data) == false);

    cocoa::toggle_bit<uint8_t, 3>(data);
    REQUIRE(cocoa::is_bit_set<uint8_t, 3>(data) == false);

    cocoa::toggle_bit<uint8_t, 4>(data);
    REQUIRE(cocoa::is_bit_set<uint8_t, 4>(data) == true);

    cocoa::toggle_bit<uint8_t, 5>(data);
    REQUIRE(cocoa::is_bit_set<uint8_t, 5>(data) == true);

    cocoa::toggle_bit<uint8_t, 7>(data);
    REQUIRE(cocoa::is_bit_set<uint8_t, 7>(data) == false);
}

TEST_CASE("constexpr T cocoa::from_pair(V high, V low)", "[from_pair]")
{
    uint16_t expect1 = cocoa::from_pair<uint16_t, uint8_t>(0xBE, 0xEF);
    REQUIRE(expect1 == 0xBEEF);

    uint32_t expect2 = cocoa::from_pair<uint32_t, uint16_t>(0xDEAD, 0xBEEF);
    REQUIRE(expect2 == 0xDEADBEEF);
}

TEST_CASE("constexpr T cocoa::from_high(V)", "[from_high]")
{
    uint8_t expect1 = cocoa::from_high<uint8_t, uint16_t>(0xBEEF);
    REQUIRE(expect1 == 0xBE);

    uint16_t expect2 = cocoa::from_high<uint16_t, uint32_t>(0xDEADBEEF);
    REQUIRE(expect2 == 0xDEAD);
}

TEST_CASE("constexpr T cocoa::from_low(V)", "[from_low]")
{
    uint8_t expect1 = cocoa::from_low<uint8_t, uint16_t>(0xBEEF);
    REQUIRE(expect1 == 0xEF);

    uint16_t expect2 = cocoa::from_low<uint16_t, uint32_t>(0xDEADBEEF);
    REQUIRE(expect2 == 0xBEEF);
}
