// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include <cstdint>

#include <catch2/catch_test_macros.hpp>
#include <spdlog/logger.h>

#include "cocoa/gb/memory.hpp"
#include "cocoa/gb/sm83.hpp"

TEST_CASE("constexpr uint8_t cocoa::gb::Sm83State::load_reg8()", "[load_reg8]")
{
    constexpr uint16_t c_indir_addr = 0xFF13;
    constexpr uint16_t hl_indir_addr = 0x014D;

    cocoa::gb::MemoryBus bus;
    bus.write_byte(c_indir_addr, 0x42);
    bus.write_byte(hl_indir_addr, 0xBF);

    cocoa::gb::Sm83State cpu(bus);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::B>() == 0x00);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::C>() == 0x13);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::IndirHramC>() == 0x42);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::D>() == 0x00);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::E>() == 0xD8);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::H>() == 0x01);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::L>() == 0x4D);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::IndirHL>() == 0xBF);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::A>() == 0x01);
}

TEST_CASE("constexpr void cocoa::gb::Sm83State::store_reg8(const uint8_t)", "[store_reg8]")
{
    constexpr uint8_t expect = 0x42;

    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    cpu.store_reg8<cocoa::gb::Reg8::B>(expect);
    cpu.store_reg8<cocoa::gb::Reg8::C>(expect);
    cpu.store_reg8<cocoa::gb::Reg8::IndirHramC>(expect);
    cpu.store_reg8<cocoa::gb::Reg8::D>(expect);
    cpu.store_reg8<cocoa::gb::Reg8::E>(expect);
    cpu.store_reg8<cocoa::gb::Reg8::H>(expect);
    cpu.store_reg8<cocoa::gb::Reg8::L>(expect);
    cpu.store_reg8<cocoa::gb::Reg8::IndirHL>(expect);
    cpu.store_reg8<cocoa::gb::Reg8::A>(expect);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::B>() == expect);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::C>() == expect);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::IndirHramC>() == expect);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::D>() == expect);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::E>() == expect);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::H>() == expect);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::L>() == expect);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::IndirHL>() == expect);
    REQUIRE(cpu.load_reg8<cocoa::gb::Reg8::A>() == expect);
}

TEST_CASE("constexpr uint16_t cocoa::gb::Sm83State::load_reg16()", "[load_reg16]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    REQUIRE(cpu.load_reg16<cocoa::gb::Reg16::BC>() == 0x0013);
    REQUIRE(cpu.load_reg16<cocoa::gb::Reg16::DE>() == 0x00D8);
    REQUIRE(cpu.load_reg16<cocoa::gb::Reg16::HL>() == 0x014D);
    REQUIRE(cpu.load_reg16<cocoa::gb::Reg16::SP>() == 0xFFFE);
}

TEST_CASE("constexpr uint16_t cocoa::gb::Sm83State::load_reg16_stack()", "[load_reg16_stack]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    REQUIRE(cpu.load_reg16_stack<cocoa::gb::Reg16Stack::BC>() == 0x0013);
    REQUIRE(cpu.load_reg16_stack<cocoa::gb::Reg16Stack::DE>() == 0x00D8);
    REQUIRE(cpu.load_reg16_stack<cocoa::gb::Reg16Stack::HL>() == 0x014D);
    REQUIRE(cpu.load_reg16_stack<cocoa::gb::Reg16Stack::AF>() == 0x0180);
}

TEST_CASE("constexpr uint8_t cocoa::gb::Sm83State::load_reg16_indir()", "[load_reg16_indir]")
{
    constexpr uint8_t expect = 0x42;

    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    bus.write_byte(0x0013, expect);
    bus.write_byte(0x00D8, expect);
    bus.write_byte(0x014D, expect);
    bus.write_byte(0x014E, expect);

    REQUIRE(cpu.load_reg16_indir<cocoa::gb::Reg16Indir::BC>() == expect);
    REQUIRE(cpu.load_reg16_indir<cocoa::gb::Reg16Indir::DE>() == expect);
    REQUIRE(cpu.load_reg16_indir<cocoa::gb::Reg16Indir::HLI>() == expect);
    REQUIRE(cpu.load_reg16_indir<cocoa::gb::Reg16Indir::HLD>() == expect);
}

TEST_CASE("constexpr void cocoa::gb::Sm83State::store_reg16()", "[store_reg16]")
{
    constexpr uint16_t expect = 0xBEEF;

    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    cpu.store_reg16<cocoa::gb::Reg16::BC>(expect);
    cpu.store_reg16<cocoa::gb::Reg16::DE>(expect);
    cpu.store_reg16<cocoa::gb::Reg16::HL>(expect);
    cpu.store_reg16<cocoa::gb::Reg16::SP>(expect);
    REQUIRE(cpu.load_reg16<cocoa::gb::Reg16::BC>() == expect);
    REQUIRE(cpu.load_reg16<cocoa::gb::Reg16::DE>() == expect);
    REQUIRE(cpu.load_reg16<cocoa::gb::Reg16::HL>() == expect);
    REQUIRE(cpu.load_reg16<cocoa::gb::Reg16::SP>() == expect);
}

TEST_CASE(
    "constexpr void cocoa::gb::Sm83State::store_reg16_stack(const uint16_t)", "[store_reg16_stack]")
{
    constexpr uint16_t expect = 0xBEEF;

    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    cpu.store_reg16_stack<cocoa::gb::Reg16Stack::BC>(expect);
    cpu.store_reg16_stack<cocoa::gb::Reg16Stack::DE>(expect);
    cpu.store_reg16_stack<cocoa::gb::Reg16Stack::HL>(expect);
    cpu.store_reg16_stack<cocoa::gb::Reg16Stack::AF>(expect);
    REQUIRE(cpu.load_reg16_stack<cocoa::gb::Reg16Stack::BC>() == expect);
    REQUIRE(cpu.load_reg16_stack<cocoa::gb::Reg16Stack::DE>() == expect);
    REQUIRE(cpu.load_reg16_stack<cocoa::gb::Reg16Stack::HL>() == expect);
    REQUIRE(cpu.load_reg16_stack<cocoa::gb::Reg16Stack::AF>() == expect);
}

TEST_CASE(
    "constexpr void cocoa::gb::Sm83State::store_reg16_indir(const uint8_t)", "[store_reg16_indir]")
{
    constexpr uint8_t expect = 0x42;

    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    cpu.store_reg16_indir<cocoa::gb::Reg16Indir::BC>(expect);
    cpu.store_reg16_indir<cocoa::gb::Reg16Indir::DE>(expect);
    cpu.store_reg16_indir<cocoa::gb::Reg16Indir::HLI>(expect);
    cpu.store_reg16_indir<cocoa::gb::Reg16Indir::HLD>(expect);
    REQUIRE(cpu.load_reg16_indir<cocoa::gb::Reg16Indir::BC>() == expect);
    REQUIRE(cpu.load_reg16_indir<cocoa::gb::Reg16Indir::DE>() == expect);
    REQUIRE(cpu.load_reg16_indir<cocoa::gb::Reg16Indir::HLI>() == expect);
    REQUIRE(cpu.load_reg16_indir<cocoa::gb::Reg16Indir::HLD>() == expect);
}

TEST_CASE("constexpr uint8_t cocoa::gb::Sm83State::load_imm8()", "[load_imm8]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    bus.write_byte(0x0100, 0x42);
    REQUIRE(cpu.load_imm8<cocoa::gb::Imm8::Direct>() == 0x42);

    bus.write_byte(0x0101, 0x32);
    bus.write_byte(0xFF32, 0xBF);
    REQUIRE(cpu.load_imm8<cocoa::gb::Imm8::IndirHram>() == 0xBF);

    bus.write_word(0x0102, 0xAFAF);
    bus.write_byte(0xAFAF, 0x21);
    REQUIRE(cpu.load_imm8<cocoa::gb::Imm8::IndirAbsolute>() == 0x21);
}

TEST_CASE("constexpr void cocoa::gb::Sm83State::store_imm8()", "[store_imm8]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    bus.write_byte(0x0100, 0x00);
    cpu.store_imm8<cocoa::gb::Imm8::IndirHram>(0x42);
    REQUIRE(bus.read_byte(0xFF00) == 0x42);

    bus.write_word(0x0101, 0x1234);
    cpu.store_imm8<cocoa::gb::Imm8::IndirAbsolute>(0x42);
    REQUIRE(bus.read_byte(0x1234) == 0x42);
}

TEST_CASE("constexpr uint16_t cocoa::gb::Sm83State::load_imm16()", "[load_imm16]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    bus.write_word(0x0100, 0xBEEF);
    REQUIRE(cpu.load_imm16<cocoa::gb::Imm16::Direct>() == 0xBEEF);
}

TEST_CASE("constexpr void cocoa::gb::Sm83State::store_imm16(const uint16_t)", "[store_imm16]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    bus.write_word(0x0100, 0xAFAF);
    cpu.store_imm16<cocoa::gb::Imm16::IndirAbsolute>(0x1234);
    REQUIRE(bus.read_word(0xAFAF) == 0x1234);
}

TEST_CASE("constexpr void cocoa::gb::Sm83State::set_flag", "[set_flag]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    cpu.regs[cocoa::gb::Sm83State::RegIndex::F] = 0x00000000;
    cpu.set_flag<cocoa::gb::Flag::Z>();
    cpu.set_flag<cocoa::gb::Flag::H>();
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::Z>() == true);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::N>() == false);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::H>() == true);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::C>() == false);
}

TEST_CASE("constexpr void cocoa::gb::Sm83State::clear_flag()", "[clear_flag]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    cpu.regs[cocoa::gb::Sm83State::RegIndex::F] = 0b11110000;
    cpu.clear_flag<cocoa::gb::Flag::N>();
    cpu.clear_flag<cocoa::gb::Flag::C>();
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::Z>() == true);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::N>() == false);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::H>() == true);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::C>() == false);
}

TEST_CASE(
    "constexpr void cocoa::gb::Sm83State::conditional_flag_toggle()", "[conditional_flag_toggle]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    cpu.regs[cocoa::gb::Sm83State::RegIndex::F] = 0b01110000;
    cpu.conditional_flag_toggle<cocoa::gb::Flag::Z>(true);
    cpu.conditional_flag_toggle<cocoa::gb::Flag::H>(false);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::Z>() == true);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::N>() == true);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::H>() == false);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::C>() == true);
}

TEST_CASE("constexpr void cocoa::gb::Sm83State::toggle_flag()", "[toggle_flag]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    cpu.regs[cocoa::gb::Sm83State::RegIndex::F] = 0b10100000;
    cpu.toggle_flag<cocoa::gb::Flag::Z>();
    cpu.toggle_flag<cocoa::gb::Flag::N>();
    cpu.toggle_flag<cocoa::gb::Flag::H>();
    cpu.toggle_flag<cocoa::gb::Flag::C>();
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::Z>() == false);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::N>() == true);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::H>() == false);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::C>() == true);
}

TEST_CASE("constexpr bool cocoa::gb::Sm83State::is_flag_set()", "[is_flag_set]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    cpu.regs[cocoa::gb::Sm83State::RegIndex::F] = 0b01010000;
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::Z>() == false);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::N>() == true);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::H>() == false);
    REQUIRE(cpu.is_flag_set<cocoa::gb::Flag::C>() == true);
}

TEST_CASE("constexpr bool cocoa::gb::Sm83State::is_condition_set()", "[is_condition_set]")
{
    cocoa::gb::MemoryBus bus;
    cocoa::gb::Sm83State cpu(bus);

    cpu.regs[cocoa::gb::Sm83State::RegIndex::F] = 0b10010000;
    REQUIRE(cpu.is_condition_set<cocoa::gb::Condition::NZ>() == false);
    REQUIRE(cpu.is_condition_set<cocoa::gb::Condition::NC>() == false);
    REQUIRE(cpu.is_condition_set<cocoa::gb::Condition::Z>() == true);
    REQUIRE(cpu.is_condition_set<cocoa::gb::Condition::C>() == true);

    cpu.regs[cocoa::gb::Sm83State::RegIndex::F] = 0b00000000;
    REQUIRE(cpu.is_condition_set<cocoa::gb::Condition::NZ>() == true);
    REQUIRE(cpu.is_condition_set<cocoa::gb::Condition::NC>() == true);
    REQUIRE(cpu.is_condition_set<cocoa::gb::Condition::Z>() == false);
    REQUIRE(cpu.is_condition_set<cocoa::gb::Condition::C>() == false);
}
