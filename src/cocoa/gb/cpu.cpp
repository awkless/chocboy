// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include <fmt/format.h>
#include <spdlog/logger.h>

#include "cocoa/gb/cpu.hpp"
#include "cocoa/gb/memory.hpp"

namespace cocoa::gb {
struct Opcode final {
    std::string_view mnemonic;
    size_t cycles;
    void (*execute)(Sm83State&) = nullptr;
};

enum class OpcodeKind : uint8_t {
    LoadRegBRegB = 0x40,
    LoadRegBRegC = 0x41,
    LoadRegBRegD = 0x42,
    LoadRegBRegE = 0x43,
    LoadRegBRegH = 0x44,
    LoadRegBRegL = 0x45,
    LoadRegBRegHL = 0x46,
    LoadRegBRegA = 0x47,
    LoadRegCRegB = 0x48,
    LoadRegCRegC = 0x49,
    LoadRegCRegD = 0x4A,
    LoadRegCRegE = 0x4B,
    LoadRegCRegH = 0x4C,
    LoadRegCRegL = 0x4D,
    LoadRegCRegHL = 0x4E,
    LoadRegCRegA = 0x4F,
    LoadRegDRegB = 0x50,
    LoadRegDRegC = 0x51,
    LoadRegDRegD = 0x52,
    LoadRegDRegE = 0x53,
    LoadRegDRegH = 0x54,
    LoadRegDRegL = 0x55,
    LoadRegDRegHL = 0x56,
    LoadRegDRegA = 0x57,
    LoadRegERegB = 0x58,
    LoadRegERegC = 0x59,
    LoadRegERegD = 0x5A,
    LoadRegERegE = 0x5B,
    LoadRegERegH = 0x5C,
    LoadRegERegL = 0x5D,
    LoadRegERegHL = 0x5E,
    LoadRegERegA = 0x5F,
    LoadRegHRegB = 0x60,
    LoadRegHRegC = 0x61,
    LoadRegHRegD = 0x62,
    LoadRegHRegE = 0x63,
    LoadRegHRegH = 0x64,
    LoadRegHRegL = 0x65,
    LoadRegHRegHL = 0x66,
    LoadRegHRegA = 0x67,
    LoadRegLRegB = 0x68,
    LoadRegLRegC = 0x69,
    LoadRegLRegD = 0x6A,
    LoadRegLRegE = 0x6B,
    LoadRegLRegH = 0x6C,
    LoadRegLRegL = 0x6D,
    LoadRegLRegHL = 0x6E,
    LoadRegLRegA = 0x6F,
    LoadRegHLRegB = 0x70,
    LoadRegHLRegC = 0x71,
    LoadRegHLRegD = 0x72,
    LoadRegHLRegE = 0x73,
    LoadRegHLRegH = 0x74,
    LoadRegHLRegL = 0x75,
    LoadRegHLRegA = 0x77,
    LoadRegARegB = 0x78,
    LoadRegARegC = 0x79,
    LoadRegARegD = 0x7A,
    LoadRegARegE = 0x7B,
    LoadRegARegH = 0x7C,
    LoadRegARegL = 0x7D,
    LoadRegARegHL = 0x7E,
    LoadRegARegA = 0x7F,
    LoadRegBImm8 = 0x06,
    LoadRegCImm8 = 0x0E,
    LoadRegDImm8 = 0x16,
    LoadRegEImm8 = 0x1E,
    LoadRegHImm8 = 0x26,
    LoadRegLImm8 = 0x2E,
    LoadRegHLImm8 = 0x36,
    LoadRegAImm8 = 0x3E,
    LoadRegAMemBC = 0x0A,
    LoadRegAMemDE = 0x1A,
    LoadRegAMemHLI = 0x2A,
    LoadRegAMemHLD = 0x3A,
    LoadMemBCRegA = 0x02,
    LoadMemDERegA = 0x12,
    LoadMemHLIRegA = 0x22,
    LoadMemHLDRegA = 0x32,
    LoadRegAImm16Mem = 0xFA,
    LoadImm16MemRegA = 0xEA,
    LoadHighRegARegCMem = 0xF2,
    LoadHighRegCMemRegA = 0xE2,
    LoadHighRegAImm8Mem = 0xF0,
    LoadHighImm8MemRegA = 0xE0,
    LoadRegBCImm16 = 0x01,
    LoadRegDEImm16 = 0x11,
    LoadRegHLImm16 = 0x21,
    LoadRegSPImm16 = 0x31,
    LoadImm16MemRegSP = 0x08,
    LoadRegSPRegHL = 0xF9,
    PushBC = 0xC5,
    PushDE = 0xD5,
    PushHL = 0xE5,
    PushAF = 0xF5,
    PopBC = 0xC1,
    PopDE = 0xD1,
    PopHL = 0xE1,
    PopAF = 0xF1,
    LoadRegHLRegSPOffset = 0xF8,
    AddRegB = 0x80,
    AddRegC = 0x81,
    AddRegD = 0x82,
    AddRegE = 0x83,
    AddRegH = 0x84,
    AddRegL = 0x85,
    AddRegHL = 0x86,
    AddRegA = 0x87,
    AddImm8 = 0xC6,
    AddFlagCRegB = 0x88,
    AddFlagCRegC = 0x89,
    AddFlagCRegD = 0x8A,
    AddFlagCRegE = 0x8B,
    AddFlagCRegH = 0x8C,
    AddFlagCRegL = 0x8D,
    AddFlagCRegHL = 0x8E,
    AddFlagCRegA = 0x8F,
    AddFlagCImm8 = 0xCE,
    SubRegB = 0x90,
    SubRegC = 0x91,
    SubRegD = 0x92,
    SubRegE = 0x93,
    SubRegH = 0x94,
    SubRegL = 0x95,
    SubRegHL = 0x96,
    SubRegA = 0x97,
    SubImm8 = 0xD6,
    SubFlagCRegB = 0x98,
    SubFlagCRegC = 0x99,
    SubFlagCRegD = 0x9A,
    SubFlagCRegE = 0x9B,
    SubFlagCRegH = 0x9C,
    SubFlagCRegL = 0x9D,
    SubFlagCRegHL = 0x9E,
    SubFlagCRegA = 0x9F,
    SubFlagCImm8 = 0xDE,
    CpRegB = 0xB8,
    CpRegC = 0xB8,
    CpRegD = 0xB8,
    CpRegE = 0xB8,
    CpRegH = 0xB8,
    CpRegL = 0xB8,
    CpRegHL = 0xB8,
    CpRegA = 0xB8,
    CpImm8 = 0xFE,
    IncrementRegB = 0x04,
    IncrementRegC = 0x0C,
    IncrementRegD = 0x14,
    IncrementRegE = 0x1C,
    IncrementRegH = 0x24,
    IncrementRegL = 0x2C,
    IncrementRegHL = 0x34,
    IncrementRegA = 0x3C,
    DecrementRegB = 0x05,
    DecrementRegC = 0x0E,
    DecrementRegD = 0x15,
    DecrementRegE = 0x1E,
    DecrementRegH = 0x25,
    DecrementRegL = 0x2E,
    DecrementRegHL = 0x35,
    DecrementRegA = 0x3E,
    AndRegB = 0xA0,
    AndRegC = 0xA1,
    AndRegD = 0xA2,
    AndRegE = 0xA3,
    AndRegH = 0xA4,
    AndRegL = 0xA5,
    AndRegHL = 0xA6,
    AndRegA = 0xA7,
    AndImm8 = 0xE6,
    XorRegB = 0xA8,
    XorRegC = 0xA9,
    XorRegD = 0xAA,
    XorRegE = 0xAB,
    XorRegH = 0xAC,
    XorRegL = 0xAD,
    XorRegHL = 0xAE,
    XorRegA = 0xAF,
    XorImm8 = 0xEE,
    OrRegB = 0xB0,
    OrRegC = 0xB1,
    OrRegD = 0xB2,
    OrRegE = 0xB3,
    OrRegH = 0xB4,
    OrRegL = 0xB5,
    OrRegHL = 0xB6,
    OrRegA = 0xB7,
    OrImm8 = 0xF6,
    ComplementCarryFlag = 0x3F,
    SetCarryFlag = 0x37,
    DecimalAdjustRegA = 0x27,
    ComplementRegA = 0x2F,
    IncrementBC = 0x03,
    IncrementDE = 0x13,
    IncrementHL = 0x23,
    IncrementSP = 0x33,
    DecrementBC = 0x0B,
    DecrementDE = 0x1B,
    DecrementHL = 0x2B,
    DecrementSP = 0x3B,
    AddRegHLRegBC = 0x09,
    AddRegHLRegDE = 0x19,
    AddRegHLRegHL = 0x29,
    AddRegHLRegSP = 0x39,
    AddRegSPRelative = 0xE8,
};

enum class Operation : int {
    Add,
    Sub
};

template <enum Operation OP, typename X, typename Y>
static constexpr bool is_carry(X result, Y val1)
{
    if constexpr (OP == Operation::Add) {
        return result < val1;
    } else {
        return result > val1;
    }
}

template <enum Operation OP, typename X, typename Y>
static constexpr bool is_half_carry(X val1, Y val2)
{
    if constexpr (OP == Operation::Add) {
        return (((val1 & 0x0F) + (val2 & 0x0F)) & 0x10) == 0x10;
    } else {
        return (((val1 & 0x0F) - (val2 & 0x0F)) & 0x10) == 0x10;
    }
}

template <enum Reg8 REGX, enum Reg8 REGY>
static constexpr void load_regx_regy(Sm83State& cpu)
{
    cpu.set_reg8<REGX>(cpu.get_reg8<REGY>());
}

template <enum Reg8 REGX>
static constexpr void load_regx_imm8(Sm83State& cpu)
{
    cpu.set_reg8<REGX>(cpu.bus.read_u8(cpu.pc++));
}

template <enum Reg16Mem REG16X_MEM>
static constexpr void load_rega_reg16_mem(Sm83State& cpu)
{
    cpu.set_reg8<Reg8::A>(cpu.get_reg16_mem<REG16X_MEM>());
}

template <enum Reg16Mem REGX_MEM>
static constexpr void load_reg16_mem_rega(Sm83State& cpu)
{
    cpu.set_reg16_mem<REGX_MEM>(cpu.get_reg8<Reg8::A>());
}

static void load_rega_imm16_mem(Sm83State& cpu)
{
    uint16_t addr = cpu.bus.read_u16(cpu.pc);
    cpu.pc += 2;
    cpu.set_reg8<Reg8::A>(cpu.bus.read_u8(addr));
}

static void load_imm16_mem_rega(Sm83State& cpu)
{
    uint16_t addr = cpu.bus.read_u16(cpu.pc);
    cpu.pc += 2;
    cpu.bus.write_u8(addr, cpu.get_reg8<Reg8::A>());
}

static void load_high_rega_regc_mem(Sm83State& cpu)
{
    uint16_t addr = cocoa::from_pair<uint16_t, uint8_t>(cpu.get_reg8<Reg8::C>(), 0xFF);
    cpu.set_reg8<Reg8::A>(cpu.bus.read_u8(addr));
}

static void load_high_regc_mem_rega(Sm83State& cpu)
{
    uint16_t addr = cocoa::from_pair<uint16_t, uint8_t>(cpu.get_reg8<Reg8::C>(), 0xFF);
    cpu.bus.write_u8(addr, cpu.get_reg8<Reg8::A>());
}

static void load_high_rega_imm8_mem(Sm83State& cpu)
{
    uint16_t addr = cocoa::from_pair<uint16_t, uint8_t>(cpu.bus.read_u8(cpu.pc++), 0xFF);
    cpu.set_reg8<Reg8::A>(cpu.bus.read_u8(addr));
}

static void load_high_imm8_mem_rega(Sm83State& cpu)
{
    uint16_t addr = cocoa::from_pair<uint16_t, uint8_t>(cpu.bus.read_u8(cpu.pc++), 0xFF);
    cpu.bus.write_u8(addr, cpu.get_reg8<Reg8::A>());
}

template <enum Reg16 REG16>
static constexpr void load_regx_imm16(Sm83State& cpu)
{
    uint16_t imm16 = cpu.bus.read_u16(cpu.pc);
    cpu.pc += 2;
    cpu.set_reg16<REG16>(imm16);
}

static void load_imm16_mem_regsp(Sm83State& cpu)
{
    uint16_t addr = cpu.bus.read_u16(cpu.pc);
    cpu.pc += 2;
    cpu.bus.write_u16(addr, cpu.get_reg16<Reg16::SP>());
}

static void load_regsp_reghl(Sm83State& cpu)
{
    cpu.set_reg16<Reg16::SP>(cpu.get_reg16<Reg16::HL>());
}

template <enum Reg16Stk REG16_STK>
static constexpr void push_regx(Sm83State& cpu)
{
    uint16_t addr = cpu.get_reg16<Reg16::SP>();
    cpu.bus.write_u8(addr--, cocoa::from_low(cpu.get_reg16_stk<REG16_STK>()));
    cpu.bus.write_u8(addr--, cocoa::from_high(cpu.get_reg16_stk<REG16_STK>()));
    cpu.set_reg16<Reg16::SP>(addr);
}

template <enum Reg16Stk REG16_STK>
static constexpr void pop_regx(Sm83State& cpu)
{
    uint16_t addr = cpu.get_reg16<Reg16::SP>();
    uint8_t high = cpu.bus.read_u8(addr++);
    uint8_t low = cpu.bus.read_u8(addr++);
    cpu.set_reg16_stk<REG16_STK>(cocoa::from_pair<uint16_t, uint8_t>(high, low));
}

static void load_reghl_regsp_offset(Sm83State& cpu)
{
    int8_t offset = static_cast<int8_t>(cpu.bus.read_u8(cpu.pc++));
    uint16_t regsp = cpu.get_reg16<Reg16::SP>();
    uint16_t result = static_cast<uint16_t>(regsp + offset);
    cpu.set_reg16<Reg16::HL>(result);

    cpu.clear_flag<Flag::Z>();
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(regsp, offset));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, regsp));
}

template <enum Reg8 REGX>
static constexpr void add_regx(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t regx = cpu.get_reg8<REGX>();
    uint8_t result = rega + regx;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(rega, regx));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, rega));
}

static void add_imm8(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t imm8 = cpu.bus.read_u8(cpu.pc++);
    uint8_t result = rega + imm8;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(rega, imm8));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, rega));
}

template <enum Reg8 REGX>
static constexpr void add_flagc_regx(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t regxc = cpu.get_reg8<REGX>() + cpu.is_flag_set<Flag::C>();
    uint8_t result = rega + regxc;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(rega, regxc));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, rega));
}

static void add_flagc_imm8(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t imm8c = cpu.bus.read_u8(cpu.pc++) + cpu.is_flag_set<Flag::C>();
    uint8_t result = rega + imm8c;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(rega, imm8c));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, rega));
}

template <enum Reg8 REGX>
static constexpr void sub_regx(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t regx = cpu.get_reg8<REGX>();
    uint8_t result = rega - regx;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.set_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Sub>(rega, regx));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Sub>(result, rega));
}

static void sub_imm8(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t imm8 = cpu.bus.read_u8(cpu.pc++);
    uint8_t result = rega - imm8;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.set_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Sub>(rega, imm8));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Sub>(result, rega));
}

template <enum Reg8 REGX>
static constexpr void sub_flagc_regx(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t regxc = cpu.get_reg8<REGX>() - cpu.is_flag_set<Flag::C>();
    uint8_t result = rega - regxc;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.set_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(rega, regxc));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, rega));
}

static void sub_flagc_imm8(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t imm8c = cpu.bus.read_u8(cpu.pc++) - cpu.is_flag_set<Flag::C>();
    uint8_t result = rega - imm8c;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.set_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(rega, imm8c));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, rega));
}

template <enum Reg8 REGX>
static constexpr void cp_regx(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t regx = cpu.get_reg8<REGX>();
    uint8_t result = rega - regx;

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.set_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(rega, regx));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, rega));
}

static void cp_imm8(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t imm8 = cpu.bus.read_u8(cpu.pc++);
    uint8_t result = rega - imm8;

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.set_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Sub>(rega, imm8));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Sub>(result, rega));
}

template <enum Reg8 REGX>
static constexpr void increment_regx(Sm83State& cpu)
{
    uint8_t regx = cpu.get_reg8<REGX>();
    uint8_t result = regx + 1;
    cpu.set_reg8<REGX>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(regx, 1));
}

template <enum Reg8 REGX>
static constexpr void decrement_regx(Sm83State& cpu)
{
    uint8_t regx = cpu.get_reg8<REGX>();
    uint8_t result = regx - 1;
    cpu.set_reg8<REGX>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.set_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Sub>(regx, 1));
}

template <enum Reg8 REGX>
static constexpr void and_regx(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t regx = cpu.get_reg8<REGX>();
    uint8_t result = rega & regx;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.set_flag<Flag::H>();
    cpu.clear_flag<Flag::C>();
}

static void and_imm8(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t imm8 = cpu.bus.read_u8(cpu.pc++);
    uint8_t result = rega & imm8;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.set_flag<Flag::H>();
    cpu.clear_flag<Flag::C>();
}

template <enum Reg8 REGX>
static constexpr void xor_regx(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t regx = cpu.get_reg8<REGX>();
    uint8_t result = rega ^ regx;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.clear_flag<Flag::C>();
}

static void xor_imm8(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t imm8 = cpu.bus.read_u8(cpu.pc++);
    uint8_t result = rega ^ imm8;
    cpu.set_reg8<Reg8::A>(result);

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.clear_flag<Flag::C>();
}

template <enum Reg8 REGX>
static constexpr void or_regx(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t regx = cpu.get_reg8<REGX>();
    uint8_t result = rega | regx;

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.clear_flag<Flag::C>();
}

static void or_imm8(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    uint8_t imm8 = cpu.bus.read_u8(cpu.pc++);
    uint8_t result = rega | imm8;

    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.clear_flag<Flag::C>();
}

static void complement_carry_flag(Sm83State& cpu)
{
    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.toggle_flag<Flag::C>();
}

static void set_carry_flag(Sm83State& cpu)
{
    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.set_flag<Flag::C>();
}

static void decimal_adjust_rega(Sm83State& cpu)
{
    uint8_t rega = cpu.get_reg8<Reg8::A>();
    if (!cpu.is_flag_set<Flag::N>() || rega > 0x99) {
        rega += 0x60;
        cpu.set_flag<Flag::C>();
    } else {
        if (cpu.is_flag_set<Flag::C>()) {
            rega -= 0x60;
        }

        if (cpu.is_flag_set<Flag::H>()) {
            rega -= 0x06;
        }
    }

    cpu.set_reg8<Reg8::A>(rega);
    cpu.conditional_flag_toggle<Flag::Z>(rega == 0);
    cpu.clear_flag<Flag::H>();
}

static void complement_rega(Sm83State& cpu)
{
    cpu.set_reg8<Reg8::A>(~cpu.get_reg8<Reg8::A>());
    cpu.set_flag<Flag::N>();
    cpu.set_flag<Flag::H>();
}

template <enum Reg16 REG16X>
static constexpr void increment_reg16(Sm83State& cpu)
{
    cpu.set_reg16<REG16X>(cpu.get_reg16<REG16X>() + 1);
}

template <enum Reg16 REG16X>
static constexpr void decrement_reg16(Sm83State& cpu)
{
    cpu.set_reg16<REG16X>(cpu.get_reg16<REG16X>() - 1);
}

template <enum Reg16 REG16X>
static constexpr void add_reghl_reg16(Sm83State& cpu)
{
    uint16_t reghl = cpu.get_reg16<Reg16::HL>();
    uint16_t regx = cpu.get_reg16<REG16X>();
    uint16_t result = reghl + regx;
    cpu.set_reg16<Reg16::HL>(result);

    cpu.clear_flag<Flag::Z>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(reghl, regx));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, reghl));
}

static void add_regsp_relative(Sm83State& cpu)
{
    uint16_t regsp = cpu.get_reg16<Reg16::SP>();
    int8_t offset = static_cast<int8_t>(cpu.bus.read_u8(cpu.pc++));
    uint16_t result = static_cast<uint16_t>(regsp + offset);
    cpu.set_reg16<Reg16::SP>(result);

    cpu.clear_flag<Flag::Z>();
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(regsp, offset));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, regsp));
}

constexpr std::array<Opcode, 256> new_no_prefix_opcodes()
{
    std::array<Opcode, 256> opcodes = {};
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegBRegB)]
        = Opcode { "LD B, B", 1, load_regx_regy<Reg8::B, Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegBRegC)]
        = Opcode { "LD B, C", 1, load_regx_regy<Reg8::B, Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegBRegD)]
        = Opcode { "LD B, D", 1, load_regx_regy<Reg8::B, Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegBRegE)]
        = Opcode { "LD B, E", 1, load_regx_regy<Reg8::B, Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegBRegH)]
        = Opcode { "LD B, H", 1, load_regx_regy<Reg8::B, Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegBRegL)]
        = Opcode { "LD B, L", 1, load_regx_regy<Reg8::B, Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegBRegHL)]
        = Opcode { "LD B, [HL]", 1, load_regx_regy<Reg8::B, Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegBRegA)]
        = Opcode { "LD B, A", 1, load_regx_regy<Reg8::B, Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegCRegB)]
        = Opcode { "LD C, B", 1, load_regx_regy<Reg8::C, Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegCRegC)]
        = Opcode { "LD C, C", 1, load_regx_regy<Reg8::C, Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegCRegD)]
        = Opcode { "LD C, D", 1, load_regx_regy<Reg8::C, Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegCRegE)]
        = Opcode { "LD C, E", 1, load_regx_regy<Reg8::C, Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegCRegH)]
        = Opcode { "LD C, H", 1, load_regx_regy<Reg8::C, Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegCRegL)]
        = Opcode { "LD C, L", 1, load_regx_regy<Reg8::C, Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegCRegHL)]
        = Opcode { "LD C, [HL]", 1, load_regx_regy<Reg8::C, Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegCRegA)]
        = Opcode { "LD C, A", 1, load_regx_regy<Reg8::D, Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegDRegB)]
        = Opcode { "LD D, B", 1, load_regx_regy<Reg8::D, Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegDRegC)]
        = Opcode { "LD D, C", 1, load_regx_regy<Reg8::D, Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegDRegD)]
        = Opcode { "LD D, D", 1, load_regx_regy<Reg8::D, Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegDRegE)]
        = Opcode { "LD D, E", 1, load_regx_regy<Reg8::D, Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegDRegH)]
        = Opcode { "LD D, H", 1, load_regx_regy<Reg8::D, Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegDRegL)]
        = Opcode { "LD D, L", 1, load_regx_regy<Reg8::D, Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegDRegHL)]
        = Opcode { "LD D, [HL]", 1, load_regx_regy<Reg8::D, Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegDRegA)]
        = Opcode { "LD D, A", 1, load_regx_regy<Reg8::D, Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegERegB)]
        = Opcode { "LD E, B", 1, load_regx_regy<Reg8::E, Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegERegC)]
        = Opcode { "LD E, C", 1, load_regx_regy<Reg8::E, Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegERegD)]
        = Opcode { "LD E, D", 1, load_regx_regy<Reg8::E, Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegERegE)]
        = Opcode { "LD E, E", 1, load_regx_regy<Reg8::E, Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegERegH)]
        = Opcode { "LD E, H", 1, load_regx_regy<Reg8::E, Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegERegL)]
        = Opcode { "LD E, L", 1, load_regx_regy<Reg8::E, Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegERegHL)]
        = Opcode { "LD E, [HL]", 1, load_regx_regy<Reg8::E, Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegERegA)]
        = Opcode { "LD E, A", 1, load_regx_regy<Reg8::E, Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHRegB)]
        = Opcode { "LD H, B", 1, load_regx_regy<Reg8::H, Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHRegC)]
        = Opcode { "LD H, C", 1, load_regx_regy<Reg8::H, Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHRegD)]
        = Opcode { "LD H, D", 1, load_regx_regy<Reg8::H, Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHRegE)]
        = Opcode { "LD H, E", 1, load_regx_regy<Reg8::H, Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHRegH)]
        = Opcode { "LD H, H", 1, load_regx_regy<Reg8::H, Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHRegL)]
        = Opcode { "LD H, L", 1, load_regx_regy<Reg8::H, Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHRegHL)]
        = Opcode { "LD H, [HL]", 1, load_regx_regy<Reg8::H, Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHRegA)]
        = Opcode { "LD H, A", 1, load_regx_regy<Reg8::H, Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegLRegB)]
        = Opcode { "LD L, B", 1, load_regx_regy<Reg8::L, Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegLRegC)]
        = Opcode { "LD L, C", 1, load_regx_regy<Reg8::L, Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegLRegD)]
        = Opcode { "LD L, D", 1, load_regx_regy<Reg8::L, Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegLRegE)]
        = Opcode { "LD L, E", 1, load_regx_regy<Reg8::L, Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegLRegH)]
        = Opcode { "LD L, H", 1, load_regx_regy<Reg8::L, Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegLRegL)]
        = Opcode { "LD L, L", 1, load_regx_regy<Reg8::L, Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegLRegHL)]
        = Opcode { "LD L, [HL]", 1, load_regx_regy<Reg8::L, Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegLRegA)]
        = Opcode { "LD L, A", 1, load_regx_regy<Reg8::L, Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHLRegB)]
        = Opcode { "LD [HL], B", 2, load_regx_regy<Reg8::HL, Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHLRegC)]
        = Opcode { "LD [HL], C", 2, load_regx_regy<Reg8::HL, Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHLRegD)]
        = Opcode { "LD [HL], D", 2, load_regx_regy<Reg8::HL, Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHLRegE)]
        = Opcode { "LD [HL], E", 2, load_regx_regy<Reg8::HL, Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHLRegH)]
        = Opcode { "LD [HL], H", 2, load_regx_regy<Reg8::HL, Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHLRegL)]
        = Opcode { "LD [HL], L", 2, load_regx_regy<Reg8::HL, Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHLRegA)]
        = Opcode { "LD [HL], A", 2, load_regx_regy<Reg8::HL, Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegARegB)]
        = Opcode { "LD A, B", 1, load_regx_regy<Reg8::A, Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegARegC)]
        = Opcode { "LD A, C", 1, load_regx_regy<Reg8::A, Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegARegD)]
        = Opcode { "LD A, D", 1, load_regx_regy<Reg8::A, Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegARegE)]
        = Opcode { "LD A, E", 1, load_regx_regy<Reg8::A, Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegARegH)]
        = Opcode { "LD A, H", 1, load_regx_regy<Reg8::A, Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegARegL)]
        = Opcode { "LD A, L", 1, load_regx_regy<Reg8::A, Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegARegHL)]
        = Opcode { "LD A, [HL]", 2, load_regx_regy<Reg8::A, Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegARegA)]
        = Opcode { "LD A, A", 1, load_regx_regy<Reg8::A, Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegBImm8)]
        = Opcode { "LD B, imm8", 2, load_regx_imm8<Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegCImm8)]
        = Opcode { "LD C, imm8", 2, load_regx_imm8<Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegDImm8)]
        = Opcode { "LD D, imm8", 2, load_regx_imm8<Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegEImm8)]
        = Opcode { "LD E, imm8", 2, load_regx_imm8<Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHImm8)]
        = Opcode { "LD H, imm8", 2, load_regx_imm8<Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegLImm8)]
        = Opcode { "LD L, imm8", 2, load_regx_imm8<Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHLImm8)]
        = Opcode { "LD [HL], imm8", 2, load_regx_imm8<Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegAImm8)]
        = Opcode { "LD B, imm8", 2, load_regx_imm8<Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegAMemBC)]
        = Opcode { "LD A, [BC]", 2, load_rega_reg16_mem<Reg16Mem::BC> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegAMemDE)]
        = Opcode { "LD A, [DE]", 2, load_rega_reg16_mem<Reg16Mem::DE> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegAMemHLI)]
        = Opcode { "LD A, [HL+]", 2, load_rega_reg16_mem<Reg16Mem::HLI> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegAMemHLD)]
        = Opcode { "LD A, [HL-]", 2, load_rega_reg16_mem<Reg16Mem::HLD> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadMemBCRegA)]
        = Opcode { "LD [BC], A", 2, load_reg16_mem_rega<Reg16Mem::BC> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadMemDERegA)]
        = Opcode { "LD [DE], A", 2, load_reg16_mem_rega<Reg16Mem::DE> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadMemHLIRegA)]
        = Opcode { "LD [HL+], A", 2, load_reg16_mem_rega<Reg16Mem::HLD> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadMemHLDRegA)]
        = Opcode { "LD [HL-], A", 2, load_reg16_mem_rega<Reg16Mem::HLI> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegAImm16Mem)]
        = Opcode { "LD A, [imm16]", 4, load_rega_imm16_mem };
    opcodes[cocoa::from_enum(OpcodeKind::LoadImm16MemRegA)]
        = Opcode { "LD [imm16], A", 4, load_imm16_mem_rega };
    opcodes[cocoa::from_enum(OpcodeKind::LoadHighRegARegCMem)]
        = Opcode { "LDH A, [C]", 2, load_high_rega_regc_mem };
    opcodes[cocoa::from_enum(OpcodeKind::LoadHighRegCMemRegA)]
        = Opcode { "LDH [C], A", 2, load_high_regc_mem_rega };
    opcodes[cocoa::from_enum(OpcodeKind::LoadHighRegAImm8Mem)]
        = Opcode { "LDH A, [imm8]", 3, load_high_rega_imm8_mem };
    opcodes[cocoa::from_enum(OpcodeKind::LoadHighImm8MemRegA)]
        = Opcode { "LDH [imm8], A", 3, load_high_imm8_mem_rega };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegBCImm16)]
        = Opcode { "LD BC, imm16", 3, load_regx_imm16<Reg16::BC> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegDEImm16)]
        = Opcode { "LD DE, imm16", 3, load_regx_imm16<Reg16::DE> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHLImm16)]
        = Opcode { "LD HL, imm16", 3, load_regx_imm16<Reg16::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegSPImm16)]
        = Opcode { "LD SP, imm16", 3, load_regx_imm16<Reg16::SP> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadImm16MemRegSP)]
        = Opcode { "LD [imm16], SP", 5, load_imm16_mem_regsp };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegSPRegHL)]
        = Opcode { "LD SP, HL", 2, load_regsp_reghl };
    opcodes[cocoa::from_enum(OpcodeKind::PushBC)]
        = Opcode { "PUSH BC", 4, push_regx<Reg16Stk::BC> };
    opcodes[cocoa::from_enum(OpcodeKind::PushDE)]
        = Opcode { "PUSH DE", 4, push_regx<Reg16Stk::DE> };
    opcodes[cocoa::from_enum(OpcodeKind::PushHL)]
        = Opcode { "PUSH HL", 4, push_regx<Reg16Stk::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::PushAF)]
        = Opcode { "PUSH AF", 4, push_regx<Reg16Stk::AF> };
    opcodes[cocoa::from_enum(OpcodeKind::PopBC)]
        = Opcode { "POP BC", 3, pop_regx<Reg16Stk::BC> };
    opcodes[cocoa::from_enum(OpcodeKind::PopDE)]
        = Opcode { "POP DE", 3, pop_regx<Reg16Stk::DE> };
    opcodes[cocoa::from_enum(OpcodeKind::PopHL)]
        = Opcode { "POP HL", 3, pop_regx<Reg16Stk::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::PopAF)]
        = Opcode { "POP AF", 3, pop_regx<Reg16Stk::AF> };
    opcodes[cocoa::from_enum(OpcodeKind::LoadRegHLRegSPOffset)]
        = Opcode { "LD HL, SP + imm8", 3, load_reghl_regsp_offset };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegB)]
        = Opcode { "ADD B", 1, add_regx<Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegB)]
        = Opcode { "ADD C", 1, add_regx<Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegC)]
        = Opcode { "ADD D", 1, add_regx<Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegD)]
        = Opcode { "ADD E", 1, add_regx<Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegH)]
        = Opcode { "ADD H", 1, add_regx<Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegL)]
        = Opcode { "ADD L", 1, add_regx<Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegHL)]
        = Opcode { "ADD [HL]", 2, add_regx<Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegA)]
        = Opcode { "ADD A", 1, add_regx<Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::AddImm8)]
        = Opcode { "ADD imm8", 2, add_imm8 };
    opcodes[cocoa::from_enum(OpcodeKind::AddFlagCRegB)]
        = Opcode { "ADC B", 1, add_flagc_regx<Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::AddFlagCRegC)]
        = Opcode { "ADC C", 1, add_flagc_regx<Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::AddFlagCRegD)]
        = Opcode { "ADC D", 1, add_flagc_regx<Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::AddFlagCRegE)]
        = Opcode { "ADC E", 1, add_flagc_regx<Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::AddFlagCRegH)]
        = Opcode { "ADC H", 1, add_flagc_regx<Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::AddFlagCRegL)]
        = Opcode { "ADC L", 1, add_flagc_regx<Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::AddFlagCRegHL)]
        = Opcode { "ADC [HL]", 2, add_flagc_regx<Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::AddFlagCRegA)]
        = Opcode { "ADC A", 1, add_flagc_regx<Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::AddFlagCImm8)]
        = Opcode { "ADC imm8", 2, add_flagc_imm8 };
    opcodes[cocoa::from_enum(OpcodeKind::SubRegB)]
        = Opcode { "SUB B", 1, sub_regx<Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::SubRegC)]
        = Opcode { "SUB C", 1, sub_regx<Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::SubRegD)]
        = Opcode { "SUB D", 1, sub_regx<Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::SubRegE)]
        = Opcode { "SUB E", 1, sub_regx<Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::SubRegH)]
        = Opcode { "SUB H", 1, sub_regx<Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::SubRegL)]
        = Opcode { "SUB L", 1, sub_regx<Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::SubRegB)]
        = Opcode { "SUB [HL]", 2, sub_regx<Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::SubRegA)]
        = Opcode { "SUB A", 1, sub_regx<Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::SubImm8)]
        = Opcode { "SUB imm8", 2, sub_imm8 };
    opcodes[cocoa::from_enum(OpcodeKind::SubFlagCRegB)]
        = Opcode { "SBC B", 1, sub_flagc_regx<Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::SubFlagCRegC)]
        = Opcode { "SBC C", 1, sub_flagc_regx<Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::SubFlagCRegD)]
        = Opcode { "SBC D", 1, sub_flagc_regx<Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::SubFlagCRegE)]
        = Opcode { "SBC E", 1, sub_flagc_regx<Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::SubFlagCRegH)]
        = Opcode { "SBC H", 1, sub_flagc_regx<Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::SubFlagCRegL)]
        = Opcode { "SBC L", 1, sub_flagc_regx<Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::SubFlagCRegB)]
        = Opcode { "SBC [HL]", 2, sub_flagc_regx<Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::SubFlagCRegA)]
        = Opcode { "SBC A", 1, sub_flagc_regx<Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::SubFlagCImm8)]
        = Opcode { "SBC imm8", 2, sub_flagc_imm8 };
    opcodes[cocoa::from_enum(OpcodeKind::CpRegB)]
        = Opcode { "CP B", 1, cp_regx<Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::CpRegC)]
        = Opcode { "CP C", 1, cp_regx<Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::CpRegD)]
        = Opcode { "CP D", 1, cp_regx<Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::CpRegE)]
        = Opcode { "CP E", 1, cp_regx<Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::CpRegH)]
        = Opcode { "CP H", 1, cp_regx<Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::CpRegL)]
        = Opcode { "CP L", 1, cp_regx<Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::CpRegB)]
        = Opcode { "CP [HL]", 2, cp_regx<Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::CpRegA)]
        = Opcode { "CP A", 1, cp_regx<Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::CpImm8)]
        = Opcode { "CP imm8", 2, cp_imm8 };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementRegB)]
        = Opcode { "INC B", 1, increment_regx<Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementRegC)]
        = Opcode { "INC C", 1, increment_regx<Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementRegD)]
        = Opcode { "INC D", 1, increment_regx<Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementRegE)]
        = Opcode { "INC E", 1, increment_regx<Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementRegH)]
        = Opcode { "INC H", 1, increment_regx<Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementRegL)]
        = Opcode { "INC L", 1, increment_regx<Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementRegHL)]
        = Opcode { "INC [HL]", 3, increment_regx<Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementRegA)]
        = Opcode { "INC A", 1, increment_regx<Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementRegB)]
        = Opcode { "DEC B", 1, decrement_regx<Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementRegC)]
        = Opcode { "DEC C", 1, decrement_regx<Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementRegD)]
        = Opcode { "DEC D", 1, decrement_regx<Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementRegE)]
        = Opcode { "DEC E", 1, decrement_regx<Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementRegH)]
        = Opcode { "DEC H", 1, decrement_regx<Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementRegL)]
        = Opcode { "DEC L", 1, decrement_regx<Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementRegHL)]
        = Opcode { "DEC [HL]", 3, decrement_regx<Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementRegA)]
        = Opcode { "DEC A", 1, decrement_regx<Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::AndRegB)]
        = Opcode { "AND B", 1, and_regx<Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::AndRegC)]
        = Opcode { "AND C", 1, and_regx<Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::AndRegD)]
        = Opcode { "AND D", 1, and_regx<Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::AndRegE)]
        = Opcode { "AND E", 1, and_regx<Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::AndRegH)]
        = Opcode { "AND H", 1, and_regx<Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::AndRegL)]
        = Opcode { "AND L", 1, and_regx<Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::AndRegB)]
        = Opcode { "AND [HL]", 2, and_regx<Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::AndRegA)]
        = Opcode { "AND A", 1, and_regx<Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::AndImm8)]
        = Opcode { "AND imm8", 2, and_imm8 };
    opcodes[cocoa::from_enum(OpcodeKind::XorRegB)]
        = Opcode { "XOR B", 1, xor_regx<Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::XorRegC)]
        = Opcode { "XOR C", 1, xor_regx<Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::XorRegD)]
        = Opcode { "XOR D", 1, xor_regx<Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::XorRegE)]
        = Opcode { "XOR E", 1, xor_regx<Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::XorRegH)]
        = Opcode { "XOR H", 1, xor_regx<Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::XorRegL)]
        = Opcode { "XOR L", 1, xor_regx<Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::XorRegB)]
        = Opcode { "XOR [HL]", 2, xor_regx<Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::XorRegA)]
        = Opcode { "XOR A", 1, xor_regx<Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::XorImm8)]
        = Opcode { "XOR imm8", 2, xor_imm8 };
    opcodes[cocoa::from_enum(OpcodeKind::OrRegB)]
        = Opcode { "OR B", 1, or_regx<Reg8::B> };
    opcodes[cocoa::from_enum(OpcodeKind::OrRegC)]
        = Opcode { "OR C", 1, or_regx<Reg8::C> };
    opcodes[cocoa::from_enum(OpcodeKind::OrRegD)]
        = Opcode { "OR D", 1, or_regx<Reg8::D> };
    opcodes[cocoa::from_enum(OpcodeKind::OrRegE)]
        = Opcode { "OR E", 1, or_regx<Reg8::E> };
    opcodes[cocoa::from_enum(OpcodeKind::OrRegH)]
        = Opcode { "OR H", 1, or_regx<Reg8::H> };
    opcodes[cocoa::from_enum(OpcodeKind::OrRegL)]
        = Opcode { "OR L", 1, or_regx<Reg8::L> };
    opcodes[cocoa::from_enum(OpcodeKind::OrRegHL)]
        = Opcode { "OR [HL]", 2, or_regx<Reg8::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::OrRegA)]
        = Opcode { "OR A", 1, or_regx<Reg8::A> };
    opcodes[cocoa::from_enum(OpcodeKind::OrImm8)]
        = Opcode { "OR imm8", 2, or_imm8 };
    opcodes[cocoa::from_enum(OpcodeKind::ComplementCarryFlag)]
        = Opcode { "CCF", 1, complement_carry_flag };
    opcodes[cocoa::from_enum(OpcodeKind::SetCarryFlag)]
        = Opcode { "SCF", 1, set_carry_flag };
    opcodes[cocoa::from_enum(OpcodeKind::DecimalAdjustRegA)]
        = Opcode { "DAA", 1, decimal_adjust_rega };
    opcodes[cocoa::from_enum(OpcodeKind::ComplementRegA)]
        = Opcode { "CCA", 1, complement_rega };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementBC)]
        = Opcode { "INC BC", 2, increment_reg16<Reg16::BC> };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementDE)]
        = Opcode { "INC DE", 2, increment_reg16<Reg16::DE> };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementHL)]
        = Opcode { "INC HL", 2, increment_reg16<Reg16::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::IncrementSP)]
        = Opcode { "INC SP", 2, increment_reg16<Reg16::SP> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementBC)]
        = Opcode { "DEC BC", 2, decrement_reg16<Reg16::BC> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementDE)]
        = Opcode { "DEC DE", 2, decrement_reg16<Reg16::DE> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementHL)]
        = Opcode { "DEC HL", 2, decrement_reg16<Reg16::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::DecrementSP)]
        = Opcode { "DEC SP", 2, decrement_reg16<Reg16::SP> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegHLRegBC)]
        = Opcode { "ADD HL, BC", 2, add_reghl_reg16<Reg16::BC> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegHLRegDE)]
        = Opcode { "ADD HL, DE", 2, add_reghl_reg16<Reg16::DE> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegHLRegHL)]
        = Opcode { "ADD HL, HL", 2, add_reghl_reg16<Reg16::HL> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegHLRegSP)]
        = Opcode { "ADD HL, SP", 2, add_reghl_reg16<Reg16::SP> };
    opcodes[cocoa::from_enum(OpcodeKind::AddRegSPRelative)]
        = Opcode { "ADD SP, imm8", 4, add_regsp_relative };
    return opcodes;
}

Sm83State::Sm83State(MemoryBus& memory)
    : cycles(0)
    , sp(0)
    , pc(0)
    , bus(memory)
{
}

Sm83::Sm83(std::shared_ptr<spdlog::logger> log, MemoryBus& bus)
    : m_log(log)
    , m_state(bus)
{
}

void Sm83::step()
{
    constexpr std::array<Opcode, 256> no_prefix_opcodes = new_no_prefix_opcodes();

    uint8_t opcode = m_state.bus.read_u8(m_state.pc++);
    auto instr = no_prefix_opcodes[opcode];
    if (!instr.execute) {
        throw IllegalOpcode(fmt::format("Illegal opcode {1:02X} ???", opcode));
    }

    m_log->debug("Execute opcode {0:02X} {1}", opcode, instr.mnemonic);
    instr.execute(m_state);
    m_state.cycles += instr.cycles;
}

IllegalOpcode::IllegalOpcode(std::string message)
    : m_message(message)
{
}

const char* IllegalOpcode::what() const noexcept { return m_message.c_str(); }
} // namespace cocoa::gb
