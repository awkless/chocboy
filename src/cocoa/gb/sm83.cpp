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

#include "cocoa/gb/memory.hpp"
#include "cocoa/gb/sm83.hpp"
#include "cocoa/utility.hpp"

namespace cocoa::gb {
enum Load : uint8_t {
    RegBCImm16 = 0x01,
    RegDEImm16 = 0x11,
    RegHLImm16 = 0x21,
    IndirBCRegA = 0x02,
    IndirDERegA = 0x12,
    IndirHLIRegA = 0x22,
    IndirHLDRegA = 0x32,
    RegAIndirBC = 0x0A,
    RegAIndirDE = 0x1A,
    RegAIndirHLI = 0x2A,
    RegAIndirHLD = 0x3A,
    RegBImm8 = 0x06,
    RegCImm8 = 0x0E,
    RegDImm8 = 0x16,
    RegEImm8 = 0x1E,
    RegHImm8 = 0x26,
    RegLImm8 = 0x2E,
    IndirHLImm8 = 0x36,
    RegAImm8 = 0x3E,
    RegBRegB = 0x40,
    RegBRegC = 0x41,
    RegBRegD = 0x42,
    RegBRegE = 0x43,
    RegBRegH = 0x44,
    RegBRegL = 0x45,
    RegBIndirHL = 0x46,
    RegBRegA = 0x47,
    RegCRegB = 0x48,
    RegCRegC = 0x49,
    RegCRegD = 0x4A,
    RegCRegE = 0x4B,
    RegCRegH = 0x4C,
    RegCRegL = 0x4D,
    RegCIndirHL = 0x4E,
    RegCRegA = 0x4F,
    RegDRegB = 0x50,
    RegDRegC = 0x51,
    RegDRegD = 0x52,
    RegDRegE = 0x53,
    RegDRegH = 0x54,
    RegDRegL = 0x55,
    RegDIndirHL = 0x56,
    RegDRegA = 0x57,
    RegERegB = 0x58,
    RegERegC = 0x59,
    RegERegD = 0x5A,
    RegERegE = 0x5B,
    RegERegH = 0x5C,
    RegERegL = 0x5D,
    RegEIndirHL = 0x5E,
    RegERegA = 0x5F,
    RegHRegB = 0x60,
    RegHRegC = 0x61,
    RegHRegD = 0x62,
    RegHRegE = 0x63,
    RegHRegH = 0x64,
    RegHRegL = 0x65,
    RegHIndirHL = 0x66,
    RegHRegA = 0x67,
    RegLRegB = 0x68,
    RegLRegC = 0x69,
    RegLRegD = 0x6A,
    RegLRegE = 0x6B,
    RegLRegH = 0x6C,
    RegLRegL = 0x6D,
    RegLIndirHL = 0x6E,
    RegLRegA = 0x6F,
    IndirHLRegB = 0x70,
    IndirHLRegC = 0x71,
    IndirHLRegD = 0x72,
    IndirHLRegE = 0x73,
    IndirHLRegH = 0x74,
    IndirHLRegL = 0x75,
    IndirHLRegA = 0x77,
    RegARegB = 0x78,
    RegARegC = 0x79,
    RegARegD = 0x7A,
    RegARegE = 0x7B,
    RegARegH = 0x7C,
    RegARegL = 0x7D,
    RegAIndirHL = 0x7E,
    RegARegA = 0x7F,
    HramImm8RegA = 0xE0,
    HramRegAImm8 = 0xF0,
    HramIndirCRegA = 0xE2,
    HramRegAIndirC = 0xF2,
    IndirImm16RegA = 0xEA,
    RegAIndirImm16 = 0xFA,
};

enum Stack : uint8_t {
    AddRegHLRegSP = 0x39,
    AddRegSPOffset = 0xE8,
    DecRegSP = 0x38,
    IncRegSP = 0x33,
    IndirImm16RegSP = 0x08,
    RegSPImm16 = 0x31,
    PopRegBC = 0xC1,
    PopRegDE = 0xD1,
    PopRegHL = 0xE1,
    PopRegAF = 0xF1,
    PushRegBC = 0xC5,
    PushRegDE = 0xD5,
    PushRegHL = 0xE5,
    PushRegAF = 0xF5,
    RegHLRegSPOffset = 0xF8,
    RegSPRegHL = 0xF9,
};

enum Math : uint8_t {
    IncRegB = 0x04,
    IncRegC = 0x0C,
    IncRegD = 0x14,
    IncRegE = 0x1C,
    IncRegH = 0x24,
    IncRegL = 0x2C,
    IncIndirHL = 0x34,
    IncRegA = 0x3C,
    DecRegB = 0x05,
    DecRegC = 0x0D,
    DecRegD = 0x15,
    DecRegE = 0x1D,
    DecRegH = 0x25,
    DecRegL = 0x2D,
    DecIndirHL = 0x35,
    DecRegA = 0x3D,
    DecimalAdjust = 0x27,
    SetCarry = 0x37,
    ComplementCarry = 0x3F,
    IncRegBC = 0x03,
    IncRegDE = 0x13,
    IncRegHL = 0x23,
    DecRegBC = 0x0B,
    DecRegDE = 0x1B,
    DecRegHL = 0x2B,
    AddRegHLRegBC = 0x09,
    AddRegHLRegDE = 0x19,
    AddRegHLRegHL = 0x29,
    AddRegB = 0x80,
    AddRegC = 0x81,
    AddRegD = 0x82,
    AddRegE = 0x83,
    AddRegH = 0x84,
    AddRegL = 0x85,
    AddIndirHL = 0x86,
    AddRegA = 0x87,
    AddCarryRegB = 0x88,
    AddCarryRegC = 0x89,
    AddCarryRegD = 0x8A,
    AddCarryRegE = 0x8B,
    AddCarryRegH = 0x8C,
    AddCarryRegL = 0x8D,
    AddCarryIndirHL = 0x8E,
    AddCarryRegA = 0x8F,
    SubRegB = 0x90,
    SubRegC = 0x91,
    SubRegD = 0x92,
    SubRegE = 0x93,
    SubRegH = 0x94,
    SubRegL = 0x95,
    SubIndirHL = 0x96,
    SubRegA = 0x97,
    SubCarryRegB = 0x98,
    SubCarryRegC = 0x99,
    SubCarryRegD = 0x9A,
    SubCarryRegE = 0x9B,
    SubCarryRegH = 0x9C,
    SubCarryRegL = 0x9D,
    SubCarryIndirHL = 0x9E,
    SubCarryRegA = 0x9F,
    AddImm8 = 0xC6,
    AddCarryImm8 = 0xCE,
    SubImm8 = 0xD6,
    SubCarryImm8 = 0xDE,
};

enum BitLogic : uint8_t {
    ComplementRegA = 0x2F,
    AndRegB = 0xA0,
    AndRegC = 0xA1,
    AndRegD = 0xA2,
    AndRegE = 0xA3,
    AndRegH = 0xA4,
    AndRegL = 0xA5,
    AndIndirHL = 0xA6,
    AndRegA = 0xA7,
    XorRegB = 0xA8,
    XorRegC = 0xA9,
    XorRegD = 0xAA,
    XorRegE = 0xAB,
    XorRegH = 0xAC,
    XorRegL = 0xAD,
    XorIndirHL = 0xAE,
    XorRegA = 0xAF,
    OrRegB = 0xB0,
    OrRegC = 0xB1,
    OrRegD = 0xB2,
    OrRegE = 0xB3,
    OrRegH = 0xB4,
    OrRegL = 0xB5,
    OrIndirHL = 0xB6,
    OrRegA = 0xB7,
    CpRegB = 0xB8,
    CpRegC = 0xB9,
    CpRegD = 0xBA,
    CpRegE = 0xBB,
    CpRegH = 0xBC,
    CpRegL = 0xBD,
    CpIndirHL = 0xBE,
    CpRegA = 0xBF,
    AndImm8 = 0xE6,
    XorImm8 = 0xEE,
    OrImm8 = 0xF6,
    CpImm8 = 0xFE,
};

enum BitFlag : uint8_t {
    Bit0RegB = 0x40,
    Bit0RegC = 0x41,
    Bit0RegD = 0x42,
    Bit0RegE = 0x43,
    Bit0RegH = 0x44,
    Bit0RegL = 0x45,
    Bit0IndirHL = 0x46,
    Bit0RegA = 0x47,
    Bit1RegB = 0x48,
    Bit1RegC = 0x49,
    Bit1RegD = 0x4A,
    Bit1RegE = 0x4B,
    Bit1RegH = 0x4C,
    Bit1RegL = 0x4D,
    Bit1IndirHL = 0x4E,
    Bit1RegA = 0x4F,
    Bit2RegB = 0x50,
    Bit2RegC = 0x51,
    Bit2RegD = 0x52,
    Bit2RegE = 0x53,
    Bit2RegH = 0x54,
    Bit2RegL = 0x55,
    Bit2IndirHL = 0x56,
    Bit2RegA = 0x57,
    Bit3RegB = 0x58,
    Bit3RegC = 0x59,
    Bit3RegD = 0x5A,
    Bit3RegE = 0x5B,
    Bit3RegH = 0x5C,
    Bit3RegL = 0x5D,
    Bit3IndirHL = 0x5E,
    Bit3RegA = 0x5F,
    Bit4RegB = 0x60,
    Bit4RegC = 0x61,
    Bit4RegD = 0x62,
    Bit4RegE = 0x63,
    Bit4RegH = 0x64,
    Bit4RegL = 0x65,
    Bit4IndirHL = 0x66,
    Bit4RegA = 0x67,
    Bit5RegB = 0x68,
    Bit5RegC = 0x69,
    Bit5RegD = 0x6A,
    Bit5RegE = 0x6B,
    Bit5RegH = 0x6C,
    Bit5RegL = 0x6D,
    Bit5IndirHL = 0x6E,
    Bit5RegA = 0x6F,
    Bit6RegB = 0x70,
    Bit6RegC = 0x71,
    Bit6RegD = 0x72,
    Bit6RegE = 0x73,
    Bit6RegH = 0x74,
    Bit6RegL = 0x75,
    Bit6IndirHL = 0x76,
    Bit6RegA = 0x77,
    Bit7RegB = 0x78,
    Bit7RegC = 0x79,
    Bit7RegD = 0x7A,
    Bit7RegE = 0x7B,
    Bit7RegH = 0x7C,
    Bit7RegL = 0x7D,
    Bit7IndirHL = 0x7E,
    Bit7RegA = 0x7F,
    Reset0RegB = 0x80,
    Reset0RegC = 0x81,
    Reset0RegD = 0x82,
    Reset0RegE = 0x83,
    Reset0RegH = 0x84,
    Reset0RegL = 0x85,
    Reset0IndirHL = 0x86,
    Reset0RegA = 0x87,
    Reset1RegB = 0x88,
    Reset1RegC = 0x89,
    Reset1RegD = 0x8A,
    Reset1RegE = 0x8B,
    Reset1RegH = 0x8C,
    Reset1RegL = 0x8D,
    Reset1IndirHL = 0x8E,
    Reset1RegA = 0x8F,
    Reset2RegB = 0x90,
    Reset2RegC = 0x91,
    Reset2RegD = 0x92,
    Reset2RegE = 0x93,
    Reset2RegH = 0x94,
    Reset2RegL = 0x95,
    Reset2IndirHL = 0x96,
    Reset2RegA = 0x97,
    Reset3RegB = 0x98,
    Reset3RegC = 0x99,
    Reset3RegD = 0x9A,
    Reset3RegE = 0x9B,
    Reset3RegH = 0x9C,
    Reset3RegL = 0x9D,
    Reset3IndirHL = 0x9E,
    Reset3RegA = 0x9F,
    Reset4RegB = 0xA0,
    Reset4RegC = 0xA1,
    Reset4RegD = 0xA2,
    Reset4RegE = 0xA3,
    Reset4RegH = 0xA4,
    Reset4RegL = 0xA5,
    Reset4IndirHL = 0xA6,
    Reset4RegA = 0xA7,
    Reset5RegB = 0xA8,
    Reset5RegC = 0xA9,
    Reset5RegD = 0xAA,
    Reset5RegE = 0xAB,
    Reset5RegH = 0xAC,
    Reset5RegL = 0xAD,
    Reset5IndirHL = 0xAE,
    Reset5RegA = 0xAF,
    Reset6RegB = 0xB0,
    Reset6RegC = 0xB1,
    Reset6RegD = 0xB2,
    Reset6RegE = 0xB3,
    Reset6RegH = 0xB4,
    Reset6RegL = 0xB5,
    Reset6IndirHL = 0xB6,
    Reset6RegA = 0xB7,
    Reset7RegB = 0xB8,
    Reset7RegC = 0xB9,
    Reset7RegD = 0xBA,
    Reset7RegE = 0xBB,
    Reset7RegH = 0xBC,
    Reset7RegL = 0xBD,
    Reset7IndirHL = 0xBE,
    Reset7RegA = 0xBF,
    Set0RegB = 0xC0,
    Set0RegC = 0xC1,
    Set0RegD = 0xC2,
    Set0RegE = 0xC3,
    Set0RegH = 0xC4,
    Set0RegL = 0xC5,
    Set0IndirHL = 0xC6,
    Set0RegA = 0xC7,
    Set1RegB = 0xC8,
    Set1RegC = 0xC9,
    Set1RegD = 0xCA,
    Set1RegE = 0xCB,
    Set1RegH = 0xCC,
    Set1RegL = 0xCD,
    Set1IndirHL = 0xCE,
    Set1RegA = 0xCF,
    Set2RegB = 0xD0,
    Set2RegC = 0xD1,
    Set2RegD = 0xD2,
    Set2RegE = 0xD3,
    Set2RegH = 0xD4,
    Set2RegL = 0xD5,
    Set2IndirHL = 0xD6,
    Set2RegA = 0xD7,
    Set3RegB = 0xD8,
    Set3RegC = 0xD9,
    Set3RegD = 0xDA,
    Set3RegE = 0xDB,
    Set3RegH = 0xDC,
    Set3RegL = 0xDD,
    Set3IndirHL = 0xDE,
    Set3RegA = 0xDF,
    Set4RegB = 0xE0,
    Set4RegC = 0xE1,
    Set4RegD = 0xE2,
    Set4RegE = 0xE3,
    Set4RegH = 0xE4,
    Set4RegL = 0xE5,
    Set4IndirHL = 0xE6,
    Set4RegA = 0xE7,
    Set5RegB = 0xE8,
    Set5RegC = 0xE9,
    Set5RegD = 0xEA,
    Set5RegE = 0xEB,
    Set5RegH = 0xEC,
    Set5RegL = 0xED,
    Set5IndirHL = 0xEE,
    Set5RegA = 0xEF,
    Set6RegB = 0xF0,
    Set6RegC = 0xF1,
    Set6RegD = 0xF2,
    Set6RegE = 0xF3,
    Set6RegH = 0xF4,
    Set6RegL = 0xF5,
    Set6IndirHL = 0xF6,
    Set6RegA = 0xF7,
    Set7RegB = 0xF8,
    Set7RegC = 0xF9,
    Set7RegD = 0xFA,
    Set7RegE = 0xFB,
    Set7RegH = 0xFC,
    Set7RegL = 0xFD,
    Set7IndirHL = 0xFE,
    Set7RegA = 0xFF,
};

enum BitShift : uint8_t {
    RotateRegALeftCarry = 0x07,
    RotateRegARightCarry = 0x0F,
    RotateRegALeft = 0x17,
    RotateRegARight = 0x1F,
    RotateLeftCarryRegB = 0x00,
    RotateLeftCarryRegC = 0x01,
    RotateLeftCarryRegD = 0x02,
    RotateLeftCarryRegE = 0x03,
    RotateLeftCarryRegH = 0x04,
    RotateLeftCarryRegL = 0x05,
    RotateLeftCarryIndirHL = 0x06,
    RotateLeftCarryRegA = 0x07,
    RotateRightCarryRegB = 0x08,
    RotateRightCarryRegC = 0x09,
    RotateRightCarryRegD = 0x0A,
    RotateRightCarryRegE = 0x0B,
    RotateRightCarryRegH = 0x0C,
    RotateRightCarryRegL = 0x0D,
    RotateRightCarryIndirHL = 0x0E,
    RotateRightCarryRegA = 0x0F,
    RotateLeftRegB = 0x10,
    RotateLeftRegC = 0x11,
    RotateLeftRegD = 0x12,
    RotateLeftRegE = 0x13,
    RotateLeftRegH = 0x14,
    RotateLeftRegL = 0x15,
    RotateLeftIndirHL = 0x16,
    RotateLeftRegA = 0x17,
    RotateRightRegB = 0x18,
    RotateRightRegC = 0x19,
    RotateRightRegD = 0x1A,
    RotateRightRegE = 0x1B,
    RotateRightRegH = 0x1C,
    RotateRightRegL = 0x1D,
    RotateRightIndirHL = 0x1E,
    RotateRightRegA = 0x1F,
    ShiftLeftArithRegB = 0x20,
    ShiftLeftArithRegC = 0x21,
    ShiftLeftArithRegD = 0x22,
    ShiftLeftArithRegE = 0x23,
    ShiftLeftArithRegH = 0x24,
    ShiftLeftArithRegL = 0x25,
    ShiftLeftArithIndirHL = 0x26,
    ShiftLeftArithRegA = 0x27,
    ShiftRightArithRegB = 0x28,
    ShiftRightArithRegC = 0x29,
    ShiftRightArithRegD = 0x2A,
    ShiftRightArithRegE = 0x2B,
    ShiftRightArithRegH = 0x2C,
    ShiftRightArithRegL = 0x2D,
    ShiftRightArithIndirHL = 0x2E,
    ShiftRightArithRegA = 0x2F,
    SwapRegB = 0x30,
    SwapRegC = 0x31,
    SwapRegD = 0x32,
    SwapRegE = 0x33,
    SwapRegH = 0x34,
    SwapRegL = 0x35,
    SwapIndirHL = 0x36,
    SwapRegA = 0x37,
    ShiftRightLogicRegB = 0x38,
    ShiftRightLogicRegC = 0x39,
    ShiftRightLogicRegD = 0x3A,
    ShiftRightLogicRegE = 0x3B,
    ShiftRightLogicRegH = 0x3C,
    ShiftRightLogicRegL = 0x3D,
    ShiftRightLogicIndirHL = 0x3E,
    ShiftRightLogicRegA = 0x3F,
};

enum CtrlFlow : uint8_t {
    JumpImm16 = 0xC3,
    JumpRegHL = 0xE9,
    JumpNZImm16 = 0xC2,
    JumpNCImm16 = 0xD2,
    JumpZImm16 = 0xCA,
    JumpCImm16 = 0xDA,
    JumpRelImm8 = 0x18,
    JumpNZRelImm8 = 0x20,
    JumpNCRelImm8 = 0x30,
    JumpZRelImm8 = 0x28,
    JumpCRelImm8 = 0x38,
    CallImm16 = 0xCD,
    CallNZImm16 = 0xC4,
    CallNCImm16 = 0xD4,
    CallZImm16 = 0xCC,
    CallCImm16 = 0xEC,
    Return = 0xC9,
    ReturnNZ = 0xC0,
    ReturnNC = 0xD0,
    ReturnZ = 0xC8,
    ReturnC = 0xD8,
    ReturnIR = 0xD9,
    Restart00 = 0xC7,
    Restart10 = 0xD7,
    Restart20 = 0xE7,
    Restart30 = 0xF7,
    Restart08 = 0xCF,
    Restart18 = 0xDF,
    Restart28 = 0xEF,
    Restart38 = 0xFF,
};

enum Misc : uint8_t {
    Nop = 0x00,
    Stop = 0x01,
    Halt = 0x76,
    DisableIR = 0xF3,
    EnableIR = 0xF8,
    Prefix = 0xCB,
    Illegal0 = 0xD3,
    Illegal1 = 0xE3,
    Illegal2 = 0xE4,
    Illegal3 = 0xF4,
    Illegal4 = 0xDB,
    Illegal5 = 0xEB,
    Illegal6 = 0xEC,
    Illegal7 = 0xEF,
    Illegal8 = 0xDD,
    Illegal9 = 0xDE,
    IllegalA = 0xDF,
};

enum class Operation {
    Add,
    Sub,
};

enum class Direction {
    Left,
    Right,
};

enum class Shift {
    Logical,
    Arithmatic,
};

enum class UseZero {
    Yes,
    No,
};

enum class UseCarry {
    No,
    Yes,
};

template <enum Operation O, typename X, typename Y>
static constexpr bool
is_carry(X result, Y operand1)
{
    if constexpr (O == Operation::Add)
        return result < operand1;
    if constexpr (O == Operation::Sub)
        return result > operand1;
}

template <enum Operation O, typename X, typename Y>
static constexpr bool
is_half_carry(X operand1, Y operand2)
{
    if constexpr (O == Operation::Add)
        return (((operand1 & 0x0F) + (operand2 & 0x0F)) & 0x10) == 0x10;
    if constexpr (O == Operation::Sub)
        return (((operand1 & 0x0F) - (operand2 & 0x0F)) & 0x10) == 0x10;
}

template <enum Reg8 Dst, enum Reg8 Src>
static constexpr void
load(Sm83State& cpu)
{
    cpu.store_reg8<Dst>(cpu.load_reg8<Src>());
}

template <enum Reg8 Dst, enum Imm8 Src>
static constexpr void
load(Sm83State& cpu)
{
    cpu.store_reg8<Dst>(cpu.load_imm8<Src>());
}

template <enum Imm8 Dst, enum Reg8 Src>
static constexpr void
load(Sm83State& cpu)
{
    cpu.store_imm8<Dst>(cpu.load_reg8<Src>());
}

template <enum Imm8 Dst, enum Reg16 Src>
static constexpr void
load(Sm83State& cpu)
{
    cpu.store_imm8<Dst>(cpu.load_reg16<Src>());
}

template <enum Reg16 Dst, enum Imm16 Src>
static constexpr void
load(Sm83State& cpu)
{
    cpu.store_reg16<Dst>(cpu.load_imm16<Src>());
}

template <enum Imm16 Dst, enum Reg16 Src>
static constexpr void
load(Sm83State& cpu)
{
    cpu.store_imm16<Dst>(cpu.load_reg16<Src>());
}

template <enum Reg16Indir Dst, enum Reg8 Src>
static constexpr void
load(Sm83State& cpu)
{
    cpu.store_reg16_indir<Dst>(cpu.load_reg8<Src>());
}

template <enum Reg8 Dst, enum Reg16Indir Src>
static constexpr void
load(Sm83State& cpu)
{
    cpu.store_reg8<Dst>(cpu.load_reg16_indir<Src>());
}

template <enum Reg16 Dst, enum Reg16 Src>
static constexpr void
load(Sm83State& cpu)
{
    cpu.store_reg16<Dst>(cpu.load_reg16<Src>());
}

static void
load_hl_sp_offset(Sm83State& cpu)
{
    int8_t offset = static_cast<int8_t>(cpu.load_imm8<Imm8::Direct>());
    uint16_t result = static_cast<uint16_t>(cpu.sp + offset);
    cpu.store_reg16<Reg16::HL>(result);
    cpu.clear_flag<Flag::Z>();
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(cpu.sp, offset));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, cpu.sp));
}

template <enum Reg16Stack Src>
static constexpr void
push(Sm83State& cpu)
{
    uint16_t reg16 = cpu.load_reg16_stack<Src>();
    cpu.bus.write_byte(--cpu.sp, cocoa::from_low(reg16));
    cpu.bus.write_byte(--cpu.sp, cocoa::from_high(reg16));
}

template <enum Reg16Stack Dst>
static constexpr void
pop(Sm83State& cpu)
{
    uint8_t high = cpu.bus.read_byte(cpu.sp++);
    uint8_t low = cpu.bus.read_byte(cpu.sp++);
    cpu.store_reg16_stack<Dst>(cocoa::from_pair(high, low));
}

template <enum Reg8 Dst>
static constexpr void
inc(Sm83State& cpu)
{
    uint8_t operand = cpu.load_reg8<Dst>();
    uint8_t result = operand + 1;
    cpu.store_reg8<Dst>(result);
    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(operand, 1));
}

template <enum Reg8 Dst>
static constexpr void
dec(Sm83State& cpu)
{
    uint8_t operand = cpu.load_reg8<Dst>();
    uint8_t result = operand - 1;
    cpu.store_reg8<Dst>(result);
    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.set_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Sub>(operand, 1));
}

template <enum Reg16 Dst>
static constexpr void
inc(Sm83State& cpu)
{
    cpu.store_reg16<Dst>(cpu.load_reg16<Dst>() + 1);
}

template <enum Reg16 Dst>
static constexpr void
dec(Sm83State& cpu)
{
    cpu.store_reg16<Dst>(cpu.load_reg16<Dst>() - 1);
}

static inline constexpr void
add_update_flags(
    Sm83State& cpu, const uint8_t result, const uint8_t operand1, const uint8_t operand2)
{
    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(operand1, operand2));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, operand1));
}

template <enum Reg8 Src, enum UseCarry C>
static constexpr void
add_a(Sm83State& cpu)
{
    uint8_t operand1 = cpu.load_reg8<Reg8::A>();
    uint8_t operand2 = 0;

    if constexpr (C == UseCarry::Yes)
        operand2 = cpu.load_reg8<Src>() + cpu.is_flag_set<Flag::C>();
    else
        operand2 = cpu.load_reg8<Src>();

    uint8_t result = operand1 + operand2;
    cpu.store_reg8<Reg8::A>(result);
    add_update_flags(cpu, result, operand1, operand2);
}

template <enum Imm8 Src, enum UseCarry C>
static constexpr void
add_a(Sm83State& cpu)
{
    uint8_t operand1 = cpu.load_reg8<Reg8::A>();
    uint8_t operand2 = 0;

    if constexpr (C == UseCarry::Yes)
        operand2 = cpu.load_imm8<Src>() + cpu.is_flag_set<Flag::C>();
    else
        operand2 = cpu.load_imm8<Src>();

    uint8_t result = operand1 + operand2;
    cpu.store_reg8<Reg8::A>(result);
    add_update_flags(cpu, result, operand1, operand2);
}

static inline constexpr void
sub_update_flags(
    Sm83State& cpu, const uint8_t result, const uint8_t operand1, const uint8_t operand2)
{
    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.set_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Sub>(operand1, operand2));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Sub>(result, operand1));
}

template <enum Reg8 Src, enum UseCarry C>
static constexpr void
sub_a(Sm83State& cpu)
{
    uint8_t operand1 = cpu.load_reg8<Reg8::A>();
    uint8_t operand2 = 0;

    if constexpr (C == UseCarry::Yes)
        operand2 = cpu.load_reg8<Src>() - cpu.is_flag_set<Flag::C>();
    else
        operand2 = cpu.load_reg8<Src>();

    uint8_t result = operand1 - operand2;
    cpu.store_reg8<Reg8::A>(result);
    sub_update_flags(cpu, result, operand1, operand2);
}

template <enum Imm8 Src, enum UseCarry C>
static constexpr void
sub_a(Sm83State& cpu)
{
    uint8_t operand1 = cpu.load_reg8<Reg8::A>();
    uint8_t operand2 = 0;

    if constexpr (C == UseCarry::Yes)
        operand2 = cpu.load_imm8<Src>() - cpu.is_flag_set<Flag::C>();
    else
        operand2 = cpu.load_imm8<Src>();

    uint8_t result = operand1 - operand2;
    cpu.store_reg8<Reg8::A>(result);
    sub_update_flags(cpu, result, operand1, operand2);
}

static void
add_sp_offset(Sm83State& cpu)
{
    uint16_t operand1 = cpu.sp;
    int8_t operand2 = static_cast<int8_t>(cpu.load_imm8<Imm8::Direct>());
    uint16_t result = static_cast<uint16_t>(operand1 + operand2);
    cpu.sp = result;
    cpu.clear_flag<Flag::Z>();
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(operand1, operand2));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, operand1));
}

template <enum Reg16 Src>
static constexpr void
add_hl(Sm83State& cpu)
{
    uint16_t operand1 = cpu.load_reg16<Reg16::HL>();
    uint16_t operand2 = cpu.load_reg16<Src>();
    uint16_t result = operand1 + operand2;
    cpu.store_reg16<Reg16::HL>(result);
    cpu.clear_flag<Flag::N>();
    cpu.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(operand1, operand2));
    cpu.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, operand1));
}

static inline constexpr void
and_update_flags(Sm83State& cpu, const uint8_t result)
{
    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.set_flag<Flag::H>();
    cpu.clear_flag<Flag::C>();
}

template <enum Reg8 Src>
static constexpr void
and_a(Sm83State& cpu)
{
    uint8_t result = cpu.load_reg8<Reg8::A>() & cpu.load_reg8<Src>();
    cpu.store_reg8<Reg8::A>(result);
    and_update_flags(cpu, result);
}

template <enum Imm8 Src>
static constexpr void
and_a(Sm83State& cpu)
{
    uint8_t operand1 = cpu.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.load_imm8<Src>();
    uint8_t result = operand1 & operand2;
    cpu.store_reg8<Reg8::A>(result);
    and_update_flags(cpu, result);
}

static inline constexpr void
or_xor_update_flags(Sm83State& cpu, const uint8_t result)
{
    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.clear_flag<Flag::C>();
}

template <enum Reg8 Src>
static constexpr void
or_a(Sm83State& cpu)
{
    uint8_t result = cpu.load_reg8<Reg8::A>() | cpu.load_reg8<Src>();
    cpu.store_reg8<Reg8::A>(result);
    or_xor_update_flags(cpu, result);
}

template <enum Imm8 Src>
static constexpr void
or_a(Sm83State& cpu)
{
    uint8_t operand1 = cpu.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.load_imm8<Src>();
    uint8_t result = operand1 | operand2;
    cpu.store_reg8<Reg8::A>(result);
    or_xor_update_flags(cpu, result);
}

template <enum Reg8 Src>
static constexpr void
xor_a(Sm83State& cpu)
{
    uint8_t result = cpu.load_reg8<Reg8::A>() ^ cpu.load_reg8<Src>();
    cpu.store_reg8<Reg8::A>(result);
    or_xor_update_flags(cpu, result);
}

template <enum Imm8 Src>
static constexpr void
xor_a(Sm83State& cpu)
{
    uint8_t operand1 = cpu.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.load_imm8<Src>();
    uint8_t result = operand1 ^ operand2;
    cpu.store_reg8<Reg8::A>(result);
    or_xor_update_flags(cpu, result);
}

template <enum Reg8 Src>
static constexpr void
cp_a(Sm83State& cpu)
{
    uint8_t operand1 = cpu.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.load_reg8<Src>();
    uint8_t result = operand1 - operand2;
    sub_update_flags(cpu, result, operand1, operand2);
}

template <enum Imm8 Src>
static constexpr void
cp_a(Sm83State& cpu)
{
    uint8_t operand1 = cpu.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.load_imm8<Src>();
    uint8_t result = operand1 - operand2;
    sub_update_flags(cpu, result, operand1, operand2);
}

static void
complement_carry_flag(Sm83State& cpu)
{
    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.toggle_flag<Flag::C>();
}

static void
set_carry_flag(Sm83State& cpu)
{
    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.set_flag<Flag::C>();
}

static void
complement_a(Sm83State& cpu)
{
    cpu.store_reg8<Reg8::A>(~cpu.load_reg8<Reg8::A>());
    cpu.set_flag<Flag::N>();
    cpu.set_flag<Flag::H>();
}

static void
decimal_adjust(Sm83State& cpu)
{
    uint8_t rega = cpu.load_reg8<Reg8::A>();
    if (!cpu.is_flag_set<Flag::N>() || rega > 0x99) {
        rega += 0x60;
        cpu.set_flag<Flag::C>();
    } else {
        if (cpu.is_flag_set<Flag::C>())
            rega -= 0x60;
        if (cpu.is_flag_set<Flag::H>())
            rega -= 0x06;
    }

    cpu.store_reg8<Reg8::A>(rega);
    cpu.conditional_flag_toggle<Flag::Z>(rega == 0);
    cpu.clear_flag<Flag::H>();
}

template <enum Reg8 Dst, enum Direction D, enum UseZero Z, enum UseCarry C>
static constexpr void
rotate(Sm83State& cpu)
{
    uint8_t carry = 0;
    uint8_t result = 0;
    uint8_t operand = cpu.load_reg8<Dst>();

    if constexpr (D == Direction::Left) {
        carry = (operand & 0x80) >> 7;
        if constexpr (C == UseCarry::No) {
            result = static_cast<uint8_t>((operand << 1) | (operand >> 7));
        } else {
            result = static_cast<uint8_t>((operand << 1) | cpu.is_flag_set<Flag::C>());
        }
    } else {
        carry = operand & 0x01;
        if constexpr (C == UseCarry::No) {
            result = static_cast<uint8_t>((operand >> 1) | (operand << 7));
        } else {
            result = static_cast<uint8_t>((operand >> 1) | (cpu.is_flag_set<Flag::C>() << 7));
        }
    }

    cpu.store_reg8<Dst>(result);

    if constexpr (Z == UseZero::No) {
        cpu.clear_flag<Flag::Z>();
    } else {
        cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    }

    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.conditional_flag_toggle<Flag::C>(carry == 1);
}

template <enum Reg8 Dst, enum Direction D, enum Shift S>
static constexpr void
shift(Sm83State& cpu)
{
    uint8_t carry = 0;
    uint8_t result = cpu.load_reg8<Dst>();

    if constexpr (D == Direction::Left) {
        carry = (result & 0x80) >> 7;
        if constexpr ((S == Shift::Logical) || (S == Shift::Arithmatic)) {
            result <<= 1;
        }
    } else {
        carry = result & 0x01;
        if constexpr (S == Shift::Logical) {
            result >>= 1;
        } else {
            result >>= 1;
            result |= (result & 0x40) << 1;
        }
    }

    cpu.store_reg8<Dst>(result);
    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.conditional_flag_toggle<Flag::C>(carry == 1);
}

template <enum Reg8 Dst>
static constexpr void
swap(Sm83State& cpu)
{
    uint8_t result = cpu.load_reg8<Dst>();
    result = static_cast<uint8_t>((result << 4) | (result >> 4));
    cpu.store_reg8<Dst>(result);
    cpu.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.clear_flag<Flag::N>();
    cpu.clear_flag<Flag::H>();
    cpu.clear_flag<Flag::C>();
}

template <size_t Bit, enum Reg8 Src>
static constexpr void
test_bit(Sm83State& cpu)
{
    uint8_t reg = cpu.load_reg8<Src>();
    cpu.conditional_flag_toggle<Flag::Z>(cocoa::is_bit_set<uint8_t, Bit>(reg));
    cpu.clear_flag<Flag::N>();
    cpu.set_flag<Flag::H>();
}

template <size_t Bit, enum Reg8 Dst>
static constexpr void
reset_bit(Sm83State& cpu)
{
    uint8_t reg = cpu.load_reg8<Dst>();
    cocoa::clear_bit<uint8_t, Bit>(reg);
    cpu.store_reg8<Dst>(reg);
}

template <size_t Bit, enum Reg8 Dst>
static constexpr void
set_bit(Sm83State& cpu)
{
    uint8_t reg = cpu.load_reg8<Dst>();
    cocoa::set_bit<uint8_t, Bit>(reg);
    cpu.store_reg8<Dst>(reg);
}

static void
jump_imm16(Sm83State& cpu)
{
    cpu.pc = cpu.load_imm16<Imm16::Direct>();
}

static void
jump_hl(Sm83State& cpu)
{
    cpu.pc = cpu.load_reg16<Reg16::HL>();
}

template <enum Condition C>
static constexpr void
jump_cond_imm16(Sm83State& cpu)
{
    uint16_t addr = cpu.load_imm16<Imm16::Direct>();
    if (cpu.is_condition_set<C>()) {
        cpu.pc = addr;
        cpu.mcycles += 1;
        cpu.tstates += 4;
    }
}

static void
jump_rel_imm8(Sm83State& cpu)
{
    int8_t offset = static_cast<int8_t>(cpu.load_imm8<Imm8::Direct>());
    cpu.pc = static_cast<uint8_t>(cpu.pc + offset);
}

template <enum Condition C>
static constexpr void
jump_cond_rel_imm8(Sm83State& cpu)
{
    int8_t offset = static_cast<int8_t>(cpu.load_imm8<Imm8::Direct>());
    if (cpu.is_condition_set<C>()) {
        cpu.pc = static_cast<uint8_t>(cpu.pc + offset);
        cpu.mcycles += 1;
        cpu.tstates += 4;
    }
}

static void
call_imm16(Sm83State& cpu)
{
    uint16_t addr = cpu.load_imm16<Imm16::Direct>();
    cpu.bus.write_byte(--cpu.sp, cocoa::from_low(cpu.pc));
    cpu.bus.write_byte(--cpu.sp, cocoa::from_high(cpu.pc));
    cpu.pc = addr;
}

template <enum Condition C>
static void
call_cond_imm16(Sm83State& cpu)
{
    uint16_t addr = cpu.load_imm16<Imm16::Direct>();
    if (cpu.is_condition_set<C>()) {
        cpu.bus.write_byte(--cpu.sp, cocoa::from_low(cpu.pc));
        cpu.bus.write_byte(--cpu.sp, cocoa::from_high(cpu.pc));
        cpu.pc = addr;
        cpu.mcycles += 3;
        cpu.tstates += 12;
    }
}

static void
return_no_cond(Sm83State& cpu)
{
    uint8_t high = cpu.bus.read_byte(cpu.sp++);
    uint8_t low = cpu.bus.read_byte(cpu.sp++);
    cpu.pc = cocoa::from_pair(high, low);
}

template <enum Condition C>
static void
return_cond(Sm83State& cpu)
{
    if (cpu.is_condition_set<C>()) {
        uint8_t high = cpu.bus.read_byte(cpu.sp++);
        uint8_t low = cpu.bus.read_byte(cpu.sp++);
        cpu.pc = cocoa::from_pair(high, low);
        cpu.mcycles += 3;
        cpu.tstates += 12;
    }
}

static void
return_interrupt(Sm83State& cpu)
{
    uint8_t high = cpu.bus.read_byte(cpu.sp++);
    uint8_t low = cpu.bus.read_byte(cpu.sp++);
    cpu.pc = cocoa::from_pair(high, low);
    cpu.ime = true;
}

template <uint8_t Vec>
static constexpr void
restart(Sm83State& cpu)
{
    cpu.bus.write_byte(--cpu.sp, cocoa::from_low(cpu.pc));
    cpu.bus.write_byte(--cpu.sp, cocoa::from_high(cpu.pc));
    cpu.pc = cocoa::from_pair<uint16_t, uint8_t>(0x00, Vec);
}

static void
nop(Sm83State& cpu)
{
    (void)cpu;
}

static void
halt(Sm83State& cpu)
{
    cpu.mode = Sm83Mode::Halted;
}

static void
stop(Sm83State& cpu)
{
    ++cpu.pc;
    cpu.mode = Sm83Mode::Stopped;
}

static void
enable_interrupt(Sm83State& cpu)
{
    cpu.ime = true;
}

static void
disable_interrupt(Sm83State& cpu)
{
    cpu.ime = false;
}

constexpr std::array<Instruction, NO_PREFIX_INSTR_TABLE_SIZE>
new_no_prefix_instr()
{
    std::array<Instruction, NO_PREFIX_INSTR_TABLE_SIZE> instr = {};
    instr[Load::RegBRegB] = Instruction { "LD B, B", 1, 1, 4, load<Reg8::B, Reg8::B> };
    instr[Load::RegBRegC] = Instruction { "LD B, C", 1, 1, 4, load<Reg8::B, Reg8::C> };
    instr[Load::RegBRegD] = Instruction { "LD B, D", 1, 1, 4, load<Reg8::B, Reg8::D> };
    instr[Load::RegBRegE] = Instruction { "LD B, E", 1, 1, 4, load<Reg8::B, Reg8::E> };
    instr[Load::RegBRegH] = Instruction { "LD B, H", 1, 1, 4, load<Reg8::B, Reg8::H> };
    instr[Load::RegBRegL] = Instruction { "LD B, L", 1, 1, 4, load<Reg8::B, Reg8::L> };
    instr[Load::RegBRegA] = Instruction { "LD C, A", 1, 1, 4, load<Reg8::B, Reg8::A> };
    instr[Load::RegCRegB] = Instruction { "LD C, B", 1, 1, 4, load<Reg8::C, Reg8::B> };
    instr[Load::RegCRegC] = Instruction { "LD C, C", 1, 1, 4, load<Reg8::C, Reg8::C> };
    instr[Load::RegCRegD] = Instruction { "LD C, D", 1, 1, 4, load<Reg8::C, Reg8::D> };
    instr[Load::RegCRegE] = Instruction { "LD C, E", 1, 1, 4, load<Reg8::C, Reg8::E> };
    instr[Load::RegCRegH] = Instruction { "LD C, H", 1, 1, 4, load<Reg8::C, Reg8::H> };
    instr[Load::RegCRegL] = Instruction { "LD C, L", 1, 1, 4, load<Reg8::C, Reg8::L> };
    instr[Load::RegCRegA] = Instruction { "LD C, A", 1, 1, 4, load<Reg8::C, Reg8::A> };
    instr[Load::RegDRegB] = Instruction { "LD D, B", 1, 1, 4, load<Reg8::D, Reg8::B> };
    instr[Load::RegDRegC] = Instruction { "LD D, C", 1, 1, 4, load<Reg8::D, Reg8::C> };
    instr[Load::RegDRegD] = Instruction { "LD D, D", 1, 1, 4, load<Reg8::D, Reg8::D> };
    instr[Load::RegDRegE] = Instruction { "LD D, E", 1, 1, 4, load<Reg8::D, Reg8::E> };
    instr[Load::RegDRegH] = Instruction { "LD D, H", 1, 1, 4, load<Reg8::D, Reg8::H> };
    instr[Load::RegDRegL] = Instruction { "LD D, L", 1, 1, 4, load<Reg8::D, Reg8::L> };
    instr[Load::RegDRegA] = Instruction { "LD D, A", 1, 1, 4, load<Reg8::D, Reg8::A> };
    instr[Load::RegERegB] = Instruction { "LD E, B", 1, 1, 4, load<Reg8::E, Reg8::B> };
    instr[Load::RegERegC] = Instruction { "LD E, C", 1, 1, 4, load<Reg8::E, Reg8::C> };
    instr[Load::RegERegD] = Instruction { "LD E, D", 1, 1, 4, load<Reg8::E, Reg8::D> };
    instr[Load::RegERegE] = Instruction { "LD E, E", 1, 1, 4, load<Reg8::E, Reg8::E> };
    instr[Load::RegERegH] = Instruction { "LD E, H", 1, 1, 4, load<Reg8::E, Reg8::H> };
    instr[Load::RegERegL] = Instruction { "LD E, L", 1, 1, 4, load<Reg8::E, Reg8::L> };
    instr[Load::RegERegA] = Instruction { "LD E, A", 1, 1, 4, load<Reg8::E, Reg8::A> };
    instr[Load::RegHRegB] = Instruction { "LD H, B", 1, 1, 4, load<Reg8::H, Reg8::B> };
    instr[Load::RegHRegC] = Instruction { "LD H, C", 1, 1, 4, load<Reg8::H, Reg8::C> };
    instr[Load::RegHRegD] = Instruction { "LD H, D", 1, 1, 4, load<Reg8::H, Reg8::D> };
    instr[Load::RegHRegE] = Instruction { "LD H, E", 1, 1, 4, load<Reg8::H, Reg8::E> };
    instr[Load::RegHRegH] = Instruction { "LD H, H", 1, 1, 4, load<Reg8::H, Reg8::H> };
    instr[Load::RegHRegL] = Instruction { "LD H, L", 1, 1, 4, load<Reg8::H, Reg8::L> };
    instr[Load::RegHRegA] = Instruction { "LD H, A", 1, 1, 4, load<Reg8::H, Reg8::A> };
    instr[Load::RegLRegB] = Instruction { "LD L, B", 1, 1, 4, load<Reg8::L, Reg8::B> };
    instr[Load::RegLRegC] = Instruction { "LD L, C", 1, 1, 4, load<Reg8::L, Reg8::C> };
    instr[Load::RegLRegD] = Instruction { "LD L, D", 1, 1, 4, load<Reg8::L, Reg8::D> };
    instr[Load::RegLRegE] = Instruction { "LD L, E", 1, 1, 4, load<Reg8::L, Reg8::E> };
    instr[Load::RegLRegH] = Instruction { "LD L, H", 1, 1, 4, load<Reg8::L, Reg8::H> };
    instr[Load::RegLRegL] = Instruction { "LD L, L", 1, 1, 4, load<Reg8::L, Reg8::L> };
    instr[Load::RegLRegA] = Instruction { "LD L, A", 1, 1, 4, load<Reg8::L, Reg8::A> };
    instr[Load::RegARegB] = Instruction { "LD A, B", 1, 1, 4, load<Reg8::A, Reg8::B> };
    instr[Load::RegARegC] = Instruction { "LD A, C", 1, 1, 4, load<Reg8::A, Reg8::C> };
    instr[Load::RegARegD] = Instruction { "LD A, D", 1, 1, 4, load<Reg8::A, Reg8::D> };
    instr[Load::RegARegE] = Instruction { "LD A, E", 1, 1, 4, load<Reg8::A, Reg8::E> };
    instr[Load::RegARegH] = Instruction { "LD A, H", 1, 1, 4, load<Reg8::A, Reg8::H> };
    instr[Load::RegARegL] = Instruction { "LD A, L", 1, 1, 4, load<Reg8::A, Reg8::L> };
    instr[Load::RegARegA] = Instruction { "LD A, A", 1, 1, 4, load<Reg8::A, Reg8::A> };
    instr[Load::RegBImm8] = Instruction { "LD B, n8", 2, 2, 8, load<Reg8::B, Imm8::Direct> };
    instr[Load::RegCImm8] = Instruction { "LD C, n8", 2, 2, 8, load<Reg8::C, Imm8::Direct> };
    instr[Load::RegDImm8] = Instruction { "LD D, n8", 2, 2, 8, load<Reg8::D, Imm8::Direct> };
    instr[Load::RegEImm8] = Instruction { "LD E, n8", 2, 2, 8, load<Reg8::E, Imm8::Direct> };
    instr[Load::RegHImm8] = Instruction { "LD H, n8", 2, 2, 8, load<Reg8::H, Imm8::Direct> };
    instr[Load::RegLImm8] = Instruction { "LD L, n8", 2, 2, 8, load<Reg8::L, Imm8::Direct> };
    instr[Load::RegAImm8] = Instruction { "LD A, n8", 2, 2, 8, load<Reg8::A, Imm8::Direct> };
    instr[Load::RegBIndirHL] = Instruction { "LD B, [HL]", 1, 2, 8, load<Reg8::B, Reg8::IndirHL> };
    instr[Load::RegCIndirHL] = Instruction { "LD C, [HL]", 1, 2, 8, load<Reg8::C, Reg8::IndirHL> };
    instr[Load::RegDIndirHL] = Instruction { "LD D, [HL]", 1, 2, 8, load<Reg8::D, Reg8::IndirHL> };
    instr[Load::RegEIndirHL] = Instruction { "LD E, [HL]", 1, 2, 8, load<Reg8::E, Reg8::IndirHL> };
    instr[Load::RegHIndirHL] = Instruction { "LD H, [HL]", 1, 2, 8, load<Reg8::H, Reg8::IndirHL> };
    instr[Load::RegLIndirHL] = Instruction { "LD L, [HL]", 1, 2, 8, load<Reg8::L, Reg8::IndirHL> };
    instr[Load::RegAIndirHL] = Instruction { "LD A, [HL]", 1, 2, 8, load<Reg8::A, Reg8::IndirHL> };
    instr[Load::IndirHLRegB] = Instruction { "LD [HL], B", 1, 2, 8, load<Reg8::IndirHL, Reg8::B> };
    instr[Load::IndirHLRegC] = Instruction { "LD [HL], C", 1, 2, 8, load<Reg8::IndirHL, Reg8::C> };
    instr[Load::IndirHLRegD] = Instruction { "LD [HL], D", 1, 2, 8, load<Reg8::IndirHL, Reg8::D> };
    instr[Load::IndirHLRegE] = Instruction { "LD [HL], E", 1, 2, 8, load<Reg8::IndirHL, Reg8::E> };
    instr[Load::IndirHLRegH] = Instruction { "LD [HL], H", 1, 2, 8, load<Reg8::IndirHL, Reg8::H> };
    instr[Load::IndirHLRegL] = Instruction { "LD [HL], L", 1, 2, 8, load<Reg8::IndirHL, Reg8::L> };
    instr[Load::IndirHLRegA] = Instruction { "LD [HL], A", 1, 2, 8, load<Reg8::IndirHL, Reg8::A> };
    instr[Load::IndirHLImm8]
        = Instruction { "LD [HL], n8", 2, 3, 12, load<Reg8::IndirHL, Imm8::Direct> };
    instr[Load::RegAIndirImm16]
        = Instruction { "LD A, [n16]", 3, 4, 16, load<Reg8::A, Imm8::IndirAbsolute> };
    instr[Load::IndirImm16RegA]
        = Instruction { "LD [n16], A", 3, 4, 16, load<Imm8::IndirAbsolute, Reg8::A> };
    instr[Load::HramRegAIndirC]
        = Instruction { "LDH A, [C]", 1, 2, 8, load<Reg8::A, Reg8::IndirHramC> };
    instr[Load::HramIndirCRegA]
        = Instruction { "LDH [C], A", 1, 2, 8, load<Reg8::IndirHramC, Reg8::A> };
    instr[Load::HramRegAImm8]
        = Instruction { "LDH A, [n8]", 2, 3, 12, load<Reg8::A, Imm8::IndirHram> };
    instr[Load::HramImm8RegA]
        = Instruction { "LDH [n8], A", 2, 3, 12, load<Imm8::IndirHram, Reg8::A> };
    instr[Load::RegBCImm16]
        = Instruction { "LD BC, n16", 3, 3, 12, load<Reg16::BC, Imm16::Direct> };
    instr[Load::RegDEImm16]
        = Instruction { "LD DE, n16", 3, 3, 12, load<Reg16::DE, Imm16::Direct> };
    instr[Load::RegHLImm16]
        = Instruction { "LD HL, n16", 3, 3, 12, load<Reg16::HL, Imm16::Direct> };
    instr[Load::IndirBCRegA] = Instruction { "LD [BC], A", 1, 2, 8, load<Reg16Indir::BC, Reg8::A> };
    instr[Load::IndirDERegA] = Instruction { "LD [DE], A", 1, 2, 8, load<Reg16Indir::DE, Reg8::A> };
    instr[Load::IndirHLIRegA]
        = Instruction { "LD [HL+], A", 1, 2, 8, load<Reg16Indir::HLI, Reg8::A> };
    instr[Load::IndirHLDRegA]
        = Instruction { "LD [HL-], A", 1, 2, 8, load<Reg16Indir::HLD, Reg8::A> };
    instr[Load::RegAIndirBC] = Instruction { "LD A, [BC]", 1, 2, 8, load<Reg8::A, Reg16Indir::BC> };
    instr[Load::RegAIndirDE] = Instruction { "LD A, [DE]", 1, 2, 8, load<Reg8::A, Reg16Indir::DE> };
    instr[Load::RegAIndirHLI]
        = Instruction { "LD A, [HLI]", 1, 2, 8, load<Reg8::A, Reg16Indir::HLI> };
    instr[Load::RegAIndirHLD]
        = Instruction { "LD A, [HLD]", 1, 2, 8, load<Reg8::A, Reg16Indir::HLD> };
    instr[Stack::RegSPImm16]
        = Instruction { "LD SP, n16", 3, 3, 12, load<Reg16::SP, Imm16::Direct> };
    instr[Stack::AddRegHLRegSP] = Instruction { "ADD HL, SP", 1, 2, 8, add_hl<Reg16::SP> };
    instr[Stack::IncRegSP] = Instruction { "INC SP", 1, 2, 8, inc<Reg16::SP> };
    instr[Stack::DecRegSP] = Instruction { "DEC SP", 1, 2, 8, dec<Reg16::SP> };
    instr[Stack::AddRegSPOffset] = Instruction { "ADD SP, e8", 2, 3, 12, add_sp_offset };
    instr[Stack::PushRegBC] = Instruction { "PUSH BC", 1, 4, 16, push<Reg16Stack::BC> };
    instr[Stack::PushRegDE] = Instruction { "PUSH DE", 1, 4, 16, push<Reg16Stack::DE> };
    instr[Stack::PushRegHL] = Instruction { "PUSH HL", 1, 4, 16, push<Reg16Stack::HL> };
    instr[Stack::PushRegAF] = Instruction { "PUSH AF", 1, 4, 16, push<Reg16Stack::AF> };
    instr[Stack::PopRegBC] = Instruction { "POP BC", 1, 3, 12, pop<Reg16Stack::BC> };
    instr[Stack::PopRegDE] = Instruction { "POP DE", 1, 3, 12, pop<Reg16Stack::DE> };
    instr[Stack::PopRegHL] = Instruction { "POP HL", 1, 3, 12, pop<Reg16Stack::HL> };
    instr[Stack::PopRegAF] = Instruction { "POP AF", 1, 3, 12, pop<Reg16Stack::AF> };
    instr[Stack::RegSPRegHL] = Instruction { "LD SP, HL", 1, 2, 8, load<Reg16::SP, Reg16::HL> };
    instr[Stack::IndirImm16RegSP]
        = Instruction { "LD [n16], SP", 3, 5, 20, load<Imm16::IndirAbsolute, Reg16::SP> };
    instr[Stack::RegHLRegSPOffset] = Instruction { "LD HL, SP + e8", 2, 3, 12, load_hl_sp_offset };
    instr[Math::AddRegB] = Instruction { "ADD A, B", 1, 1, 4, add_a<Reg8::B, UseCarry::No> };
    instr[Math::AddRegC] = Instruction { "ADD A, C", 1, 1, 4, add_a<Reg8::C, UseCarry::No> };
    instr[Math::AddRegD] = Instruction { "ADD A, D", 1, 1, 4, add_a<Reg8::D, UseCarry::No> };
    instr[Math::AddRegE] = Instruction { "ADD A, E", 1, 1, 4, add_a<Reg8::E, UseCarry::No> };
    instr[Math::AddRegH] = Instruction { "ADD A, H", 1, 1, 4, add_a<Reg8::H, UseCarry::No> };
    instr[Math::AddRegL] = Instruction { "ADD A, L", 1, 1, 4, add_a<Reg8::L, UseCarry::No> };
    instr[Math::AddRegA] = Instruction { "ADD A, A", 1, 1, 4, add_a<Reg8::A, UseCarry::No> };
    instr[Math::AddIndirHL]
        = Instruction { "ADD A, [HL]", 1, 2, 8, add_a<Reg8::IndirHL, UseCarry::No> };
    instr[Math::AddCarryRegB] = Instruction { "ADC A, B", 1, 1, 4, add_a<Reg8::B, UseCarry::Yes> };
    instr[Math::AddCarryRegC] = Instruction { "ADC A, C", 1, 1, 4, add_a<Reg8::C, UseCarry::Yes> };
    instr[Math::AddCarryRegD] = Instruction { "ADC A, D", 1, 1, 4, add_a<Reg8::D, UseCarry::Yes> };
    instr[Math::AddCarryRegE] = Instruction { "ADC A, E", 1, 1, 4, add_a<Reg8::E, UseCarry::Yes> };
    instr[Math::AddCarryRegH] = Instruction { "ADC A, H", 1, 1, 4, add_a<Reg8::H, UseCarry::Yes> };
    instr[Math::AddCarryRegL] = Instruction { "ADC A, L", 1, 1, 4, add_a<Reg8::L, UseCarry::Yes> };
    instr[Math::AddCarryRegA] = Instruction { "ADC A, A", 1, 1, 4, add_a<Reg8::A, UseCarry::Yes> };
    instr[Math::AddCarryIndirHL]
        = Instruction { "ADC A, [HL]", 1, 2, 8, add_a<Reg8::IndirHL, UseCarry::Yes> };
    instr[Math::AddImm8] = Instruction { "ADD A, n8", 2, 2, 8, add_a<Imm8::Direct, UseCarry::No> };
    instr[Math::AddCarryImm8]
        = Instruction { "ADC A, n8", 2, 2, 8, add_a<Imm8::Direct, UseCarry::Yes> };
    instr[Math::SubRegB] = Instruction { "SUB A, B", 1, 1, 4, sub_a<Reg8::B, UseCarry::No> };
    instr[Math::SubRegC] = Instruction { "SUB A, C", 1, 1, 4, sub_a<Reg8::C, UseCarry::No> };
    instr[Math::SubRegD] = Instruction { "SUB A, D", 1, 1, 4, sub_a<Reg8::D, UseCarry::No> };
    instr[Math::SubRegE] = Instruction { "SUB A, E", 1, 1, 4, sub_a<Reg8::E, UseCarry::No> };
    instr[Math::SubRegH] = Instruction { "SUB A, H", 1, 1, 4, sub_a<Reg8::H, UseCarry::No> };
    instr[Math::SubRegL] = Instruction { "SUB A, L", 1, 1, 4, sub_a<Reg8::L, UseCarry::No> };
    instr[Math::SubRegA] = Instruction { "SUB A, A", 1, 1, 4, sub_a<Reg8::A, UseCarry::No> };
    instr[Math::SubIndirHL]
        = Instruction { "SUB [HL]", 1, 2, 8, sub_a<Reg8::IndirHL, UseCarry::No> };
    instr[Math::SubCarryRegB] = Instruction { "SBC A, B", 1, 1, 4, sub_a<Reg8::B, UseCarry::Yes> };
    instr[Math::SubCarryRegC] = Instruction { "SBC A, C", 1, 1, 4, sub_a<Reg8::C, UseCarry::Yes> };
    instr[Math::SubCarryRegD] = Instruction { "SBC A, D", 1, 1, 4, sub_a<Reg8::D, UseCarry::Yes> };
    instr[Math::SubCarryRegE] = Instruction { "SBC A, E", 1, 1, 4, sub_a<Reg8::E, UseCarry::Yes> };
    instr[Math::SubCarryRegH] = Instruction { "SBC A, H", 1, 1, 4, sub_a<Reg8::H, UseCarry::Yes> };
    instr[Math::SubCarryRegL] = Instruction { "SBC A, L", 1, 1, 4, sub_a<Reg8::L, UseCarry::Yes> };
    instr[Math::SubCarryRegA] = Instruction { "SBC A, A", 1, 1, 4, sub_a<Reg8::A, UseCarry::Yes> };
    instr[Math::SubCarryIndirHL]
        = Instruction { "SBC [HL]", 1, 2, 8, sub_a<Reg8::IndirHL, UseCarry::Yes> };
    instr[Math::SubImm8] = Instruction { "SUB A, n8", 2, 2, 8, sub_a<Imm8::Direct, UseCarry::No> };
    instr[Math::SubCarryImm8]
        = Instruction { "SBC A, n8", 2, 2, 8, sub_a<Imm8::Direct, UseCarry::Yes> };
    instr[Math::IncRegB] = Instruction { "INC B", 1, 1, 4, inc<Reg8::B> };
    instr[Math::IncRegC] = Instruction { "INC C", 1, 1, 4, inc<Reg8::C> };
    instr[Math::IncRegD] = Instruction { "INC D", 1, 1, 4, inc<Reg8::D> };
    instr[Math::IncRegE] = Instruction { "INC E", 1, 1, 4, inc<Reg8::E> };
    instr[Math::IncRegH] = Instruction { "INC H", 1, 1, 4, inc<Reg8::H> };
    instr[Math::IncRegL] = Instruction { "INC L", 1, 1, 4, inc<Reg8::L> };
    instr[Math::IncRegA] = Instruction { "INC A", 1, 1, 4, inc<Reg8::A> };
    instr[Math::DecRegB] = Instruction { "DEC B", 1, 1, 4, dec<Reg8::B> };
    instr[Math::DecRegC] = Instruction { "DEC C", 1, 1, 4, dec<Reg8::C> };
    instr[Math::DecRegD] = Instruction { "DEC D", 1, 1, 4, dec<Reg8::D> };
    instr[Math::DecRegE] = Instruction { "DEC E", 1, 1, 4, dec<Reg8::E> };
    instr[Math::DecRegH] = Instruction { "DEC H", 1, 1, 4, dec<Reg8::H> };
    instr[Math::DecRegL] = Instruction { "DEC L", 1, 1, 4, dec<Reg8::L> };
    instr[Math::DecRegA] = Instruction { "DEC A", 1, 1, 4, dec<Reg8::A> };
    instr[Math::IncIndirHL] = Instruction { "INC [HL]", 1, 3, 12, inc<Reg8::IndirHL> };
    instr[Math::DecIndirHL] = Instruction { "DEC [HL]", 1, 3, 12, dec<Reg8::IndirHL> };
    instr[Math::AddRegHLRegBC] = Instruction { "ADD HL, BC", 1, 2, 8, add_hl<Reg16::HL> };
    instr[Math::AddRegHLRegDE] = Instruction { "ADD HL, BC", 1, 2, 8, add_hl<Reg16::HL> };
    instr[Math::AddRegHLRegHL] = Instruction { "ADD HL, BC", 1, 2, 8, add_hl<Reg16::HL> };
    instr[Math::IncRegBC] = Instruction { "INC BC", 1, 2, 8, inc<Reg16::BC> };
    instr[Math::IncRegDE] = Instruction { "INC DE", 1, 2, 8, inc<Reg16::DE> };
    instr[Math::IncRegHL] = Instruction { "INC HL", 1, 2, 8, inc<Reg16::HL> };
    instr[Math::DecRegBC] = Instruction { "DEC BC", 1, 2, 8, dec<Reg16::BC> };
    instr[Math::DecRegDE] = Instruction { "DEC DE", 1, 2, 8, dec<Reg16::DE> };
    instr[Math::DecRegHL] = Instruction { "DEC HL", 1, 2, 8, dec<Reg16::HL> };
    instr[Math::SetCarry] = Instruction { "SCF", 1, 1, 4, set_carry_flag };
    instr[Math::ComplementCarry] = Instruction { "CCF", 1, 1, 4, complement_carry_flag };
    instr[Math::DecimalAdjust] = Instruction { "DAA", 1, 1, 4, decimal_adjust };
    instr[BitLogic::ComplementRegA] = Instruction { "CPL", 1, 1, 4, complement_a };
    instr[BitLogic::AndRegB] = Instruction { "AND A, B", 1, 1, 4, and_a<Reg8::B> };
    instr[BitLogic::AndRegC] = Instruction { "AND A, C", 1, 1, 4, and_a<Reg8::C> };
    instr[BitLogic::AndRegD] = Instruction { "AND A, D", 1, 1, 4, and_a<Reg8::D> };
    instr[BitLogic::AndRegE] = Instruction { "AND A, E", 1, 1, 4, and_a<Reg8::E> };
    instr[BitLogic::AndRegH] = Instruction { "AND A, H", 1, 1, 4, and_a<Reg8::H> };
    instr[BitLogic::AndRegL] = Instruction { "AND A, L", 1, 1, 4, and_a<Reg8::L> };
    instr[BitLogic::AndRegA] = Instruction { "AND A, A", 1, 1, 4, and_a<Reg8::A> };
    instr[BitLogic::AndIndirHL] = Instruction { "AND A, [HL]", 1, 2, 8, and_a<Reg8::IndirHL> };
    instr[BitLogic::XorRegB] = Instruction { "XOR A, B", 1, 1, 4, xor_a<Reg8::B> };
    instr[BitLogic::XorRegC] = Instruction { "XOR A, C", 1, 1, 4, xor_a<Reg8::C> };
    instr[BitLogic::XorRegD] = Instruction { "XOR A, D", 1, 1, 4, xor_a<Reg8::D> };
    instr[BitLogic::XorRegE] = Instruction { "XOR A, E", 1, 1, 4, xor_a<Reg8::E> };
    instr[BitLogic::XorRegH] = Instruction { "XOR A, H", 1, 1, 4, xor_a<Reg8::H> };
    instr[BitLogic::XorRegL] = Instruction { "XOR A, L", 1, 1, 4, xor_a<Reg8::L> };
    instr[BitLogic::XorRegA] = Instruction { "XOR A, A", 1, 1, 4, xor_a<Reg8::A> };
    instr[BitLogic::XorIndirHL] = Instruction { "XOR A, [HL]", 1, 2, 8, xor_a<Reg8::IndirHL> };
    instr[BitLogic::OrRegB] = Instruction { "OR A, B", 1, 1, 4, or_a<Reg8::B> };
    instr[BitLogic::OrRegC] = Instruction { "OR A, C", 1, 1, 4, or_a<Reg8::C> };
    instr[BitLogic::OrRegD] = Instruction { "OR A, D", 1, 1, 4, or_a<Reg8::D> };
    instr[BitLogic::OrRegE] = Instruction { "OR A, E", 1, 1, 4, or_a<Reg8::E> };
    instr[BitLogic::OrRegH] = Instruction { "OR A, H", 1, 1, 4, or_a<Reg8::H> };
    instr[BitLogic::OrRegL] = Instruction { "OR A, L", 1, 1, 4, or_a<Reg8::L> };
    instr[BitLogic::OrRegA] = Instruction { "OR A, A", 1, 1, 4, or_a<Reg8::A> };
    instr[BitLogic::OrIndirHL] = Instruction { "OR A, [HL]", 1, 2, 8, or_a<Reg8::IndirHL> };
    instr[BitLogic::CpRegB] = Instruction { "CP A, B", 1, 1, 4, cp_a<Reg8::B> };
    instr[BitLogic::CpRegC] = Instruction { "CP A, C", 1, 1, 4, cp_a<Reg8::C> };
    instr[BitLogic::CpRegD] = Instruction { "CP A, D", 1, 1, 4, cp_a<Reg8::D> };
    instr[BitLogic::CpRegE] = Instruction { "CP A, E", 1, 1, 4, cp_a<Reg8::E> };
    instr[BitLogic::CpRegH] = Instruction { "CP A, H", 1, 1, 4, cp_a<Reg8::H> };
    instr[BitLogic::CpRegL] = Instruction { "CP A, L", 1, 1, 4, cp_a<Reg8::L> };
    instr[BitLogic::CpRegA] = Instruction { "CP A, A", 1, 1, 4, cp_a<Reg8::A> };
    instr[BitLogic::CpIndirHL] = Instruction { "CP A, [HL]", 1, 2, 8, cp_a<Reg8::IndirHL> };
    instr[BitLogic::AndImm8] = Instruction { "AND A, n8", 2, 2, 8, and_a<Imm8::Direct> };
    instr[BitLogic::XorImm8] = Instruction { "XOR A, n8", 2, 2, 8, xor_a<Imm8::Direct> };
    instr[BitLogic::OrImm8] = Instruction { "OR A, n8", 2, 2, 8, or_a<Imm8::Direct> };
    instr[BitLogic::CpImm8] = Instruction { "CP A, n8", 2, 2, 8, cp_a<Imm8::Direct> };
    instr[BitShift::RotateRegALeftCarry] = Instruction { "RLCA", 1, 1, 4,
        rotate<Reg8::A, Direction::Left, UseZero::No, UseCarry::Yes> };
    instr[BitShift::RotateRegARightCarry] = Instruction { "RRCA", 1, 1, 4,
        rotate<Reg8::A, Direction::Right, UseZero::No, UseCarry::Yes> };
    instr[BitShift::RotateRegALeft] = Instruction { "RLA", 1, 1, 4,
        rotate<Reg8::A, Direction::Left, UseZero::No, UseCarry::No> };
    instr[BitShift::RotateRegALeft] = Instruction { "RRA", 1, 1, 4,
        rotate<Reg8::A, Direction::Right, UseZero::No, UseCarry::No> };
    instr[CtrlFlow::JumpImm16] = Instruction { "JP n16", 3, 4, 16, jump_imm16 };
    instr[CtrlFlow::JumpRegHL] = Instruction { "JP HL", 1, 1, 4, jump_hl };
    instr[CtrlFlow::JumpNZImm16]
        = Instruction { "JP NZ n16", 3, 3, 12, jump_cond_imm16<Condition::NZ> };
    instr[CtrlFlow::JumpNCImm16]
        = Instruction { "JP NC n16", 3, 3, 12, jump_cond_imm16<Condition::NC> };
    instr[CtrlFlow::JumpZImm16]
        = Instruction { "JP Z n16", 3, 3, 12, jump_cond_imm16<Condition::Z> };
    instr[CtrlFlow::JumpCImm16]
        = Instruction { "JP C n16", 3, 3, 12, jump_cond_imm16<Condition::C> };
    instr[CtrlFlow::JumpRelImm8] = Instruction { "JR e8", 2, 3, 12, jump_rel_imm8 };
    instr[CtrlFlow::JumpNZRelImm8]
        = Instruction { "JR NZ e8", 2, 2, 8, jump_cond_rel_imm8<Condition::NZ> };
    instr[CtrlFlow::JumpNCRelImm8]
        = Instruction { "JR NC e8", 2, 2, 8, jump_cond_rel_imm8<Condition::NC> };
    instr[CtrlFlow::JumpZRelImm8]
        = Instruction { "JR Z e8", 2, 2, 8, jump_cond_rel_imm8<Condition::Z> };
    instr[CtrlFlow::JumpCRelImm8]
        = Instruction { "JR C e8", 2, 2, 8, jump_cond_rel_imm8<Condition::C> };
    instr[CtrlFlow::CallImm16] = Instruction { "CALL n16", 3, 6, 24, call_imm16 };
    instr[CtrlFlow::CallNZImm16]
        = Instruction { "CALL NZ n16", 3, 3, 12, call_cond_imm16<Condition::NZ> };
    instr[CtrlFlow::CallNCImm16]
        = Instruction { "CALL NC n16", 3, 3, 12, call_cond_imm16<Condition::NC> };
    instr[CtrlFlow::CallZImm16]
        = Instruction { "CALL Z n16", 3, 3, 12, call_cond_imm16<Condition::Z> };
    instr[CtrlFlow::CallCImm16]
        = Instruction { "CALL C n16", 3, 3, 12, call_cond_imm16<Condition::C> };
    instr[CtrlFlow::Return] = Instruction { "RET", 1, 4, 16, return_no_cond };
    instr[CtrlFlow::ReturnNZ] = Instruction { "RET NZ", 1, 2, 8, return_cond<Condition::NZ> };
    instr[CtrlFlow::ReturnNC] = Instruction { "RET NC", 1, 2, 8, return_cond<Condition::NC> };
    instr[CtrlFlow::ReturnZ] = Instruction { "RET Z", 1, 2, 8, return_cond<Condition::Z> };
    instr[CtrlFlow::ReturnC] = Instruction { "RET C", 1, 2, 8, return_cond<Condition::C> };
    instr[CtrlFlow::ReturnIR] = Instruction { "RETI", 1, 4, 16, return_interrupt };
    instr[CtrlFlow::Restart00] = Instruction { "RST $00", 1, 4, 16, restart<0x00> };
    instr[CtrlFlow::Restart10] = Instruction { "RST $10", 1, 4, 16, restart<0x10> };
    instr[CtrlFlow::Restart20] = Instruction { "RST $20", 1, 4, 16, restart<0x20> };
    instr[CtrlFlow::Restart30] = Instruction { "RST $30", 1, 4, 16, restart<0x30> };
    instr[CtrlFlow::Restart08] = Instruction { "RST $08", 1, 4, 16, restart<0x08> };
    instr[CtrlFlow::Restart18] = Instruction { "RST $18", 1, 4, 16, restart<0x18> };
    instr[CtrlFlow::Restart28] = Instruction { "RST $28", 1, 4, 16, restart<0x28> };
    instr[CtrlFlow::Restart38] = Instruction { "RST $38", 1, 4, 16, restart<0x38> };
    instr[Misc::Nop] = Instruction { "NOP", 1, 1, 4, nop };
    instr[Misc::Stop] = Instruction { "STOP", 2, 1, 4, stop };
    instr[Misc::Halt] = Instruction { "HALT", 1, 1, 4, halt };
    instr[Misc::EnableIR] = Instruction { "EI", 1, 1, 4, enable_interrupt };
    instr[Misc::DisableIR] = Instruction { "DI", 1, 1, 4, disable_interrupt };
    instr[Misc::Illegal0] = Instruction { "???", 1, 0, 0, nullptr };
    instr[Misc::Illegal1] = Instruction { "???", 1, 0, 0, nullptr };
    instr[Misc::Illegal2] = Instruction { "???", 1, 0, 0, nullptr };
    instr[Misc::Illegal3] = Instruction { "???", 1, 0, 0, nullptr };
    instr[Misc::Illegal4] = Instruction { "???", 1, 0, 0, nullptr };
    instr[Misc::Illegal5] = Instruction { "???", 1, 0, 0, nullptr };
    instr[Misc::Illegal6] = Instruction { "???", 1, 0, 0, nullptr };
    instr[Misc::Illegal7] = Instruction { "???", 1, 0, 0, nullptr };
    instr[Misc::Illegal8] = Instruction { "???", 1, 0, 0, nullptr };
    instr[Misc::Illegal9] = Instruction { "???", 1, 0, 0, nullptr };
    instr[Misc::IllegalA] = Instruction { "???", 1, 0, 0, nullptr };
    return instr;
}

constexpr std::array<Instruction, CB_PREFIX_INSTR_TABLE_SIZE>
new_cb_prefix_instr()
{
    std::array<Instruction, CB_PREFIX_INSTR_TABLE_SIZE> instr = {};
    instr[BitShift::RotateLeftCarryRegB] = Instruction { "RLC B", 2, 2, 8,
        rotate<Reg8::B, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftCarryRegC] = Instruction { "RLC C", 2, 2, 8,
        rotate<Reg8::C, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftCarryRegD] = Instruction { "RLC D", 2, 2, 8,
        rotate<Reg8::D, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftCarryRegE] = Instruction { "RLC E", 2, 2, 8,
        rotate<Reg8::E, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftCarryRegH] = Instruction { "RLC H", 2, 2, 8,
        rotate<Reg8::H, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftCarryRegL] = Instruction { "RLC L", 2, 2, 8,
        rotate<Reg8::L, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftCarryRegA] = Instruction { "RLC A", 2, 2, 8,
        rotate<Reg8::A, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightCarryRegB] = Instruction { "RRC B", 2, 2, 8,
        rotate<Reg8::B, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightCarryRegC] = Instruction { "RRC C", 2, 2, 8,
        rotate<Reg8::C, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightCarryRegD] = Instruction { "RRC D", 2, 2, 8,
        rotate<Reg8::D, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightCarryRegE] = Instruction { "RRC E", 2, 2, 8,
        rotate<Reg8::E, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightCarryRegH] = Instruction { "RRC H", 2, 2, 8,
        rotate<Reg8::H, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightCarryRegL] = Instruction { "RRC L", 2, 2, 8,
        rotate<Reg8::L, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightCarryRegA] = Instruction { "RRC A", 2, 2, 8,
        rotate<Reg8::A, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftRegB] = Instruction { "RL B", 2, 2, 8,
        rotate<Reg8::B, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftRegC] = Instruction { "RL C", 2, 2, 8,
        rotate<Reg8::C, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftRegD] = Instruction { "RL D", 2, 2, 8,
        rotate<Reg8::D, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftRegE] = Instruction { "RL E", 2, 2, 8,
        rotate<Reg8::E, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftRegH] = Instruction { "RL H", 2, 2, 8,
        rotate<Reg8::H, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftRegL] = Instruction { "RL L", 2, 2, 8,
        rotate<Reg8::L, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftRegA] = Instruction { "RL A", 2, 2, 8,
        rotate<Reg8::A, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightRegB] = Instruction { "RR B", 2, 2, 8,
        rotate<Reg8::B, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightRegC] = Instruction { "RR C", 2, 2, 8,
        rotate<Reg8::C, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightRegD] = Instruction { "RR D", 2, 2, 8,
        rotate<Reg8::D, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightRegE] = Instruction { "RR E", 2, 2, 8,
        rotate<Reg8::E, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightRegH] = Instruction { "RR H", 2, 2, 8,
        rotate<Reg8::H, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightRegL] = Instruction { "RR L", 2, 2, 8,
        rotate<Reg8::L, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightRegA] = Instruction { "RR A", 2, 2, 8,
        rotate<Reg8::A, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftCarryIndirHL] = Instruction { "RLC [HL]", 2, 4, 16,
        rotate<Reg8::IndirHL, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightCarryIndirHL] = Instruction { "RRC [HL]", 2, 4, 16,
        rotate<Reg8::IndirHL, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateLeftIndirHL] = Instruction { "RL [HL]", 2, 4, 16,
        rotate<Reg8::IndirHL, Direction::Left, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::RotateRightIndirHL] = Instruction { "RR [HL]", 2, 4, 16,
        rotate<Reg8::IndirHL, Direction::Right, UseZero::Yes, UseCarry::Yes> };
    instr[BitShift::ShiftLeftArithRegB]
        = Instruction { "SLA B", 2, 2, 8, shift<Reg8::B, Direction::Left, Shift::Arithmatic> };
    instr[BitShift::ShiftLeftArithRegC]
        = Instruction { "SLA C", 2, 2, 8, shift<Reg8::C, Direction::Left, Shift::Arithmatic> };
    instr[BitShift::ShiftLeftArithRegD]
        = Instruction { "SLA D", 2, 2, 8, shift<Reg8::D, Direction::Left, Shift::Arithmatic> };
    instr[BitShift::ShiftLeftArithRegE]
        = Instruction { "SLA E", 2, 2, 8, shift<Reg8::E, Direction::Left, Shift::Arithmatic> };
    instr[BitShift::ShiftLeftArithRegH]
        = Instruction { "SLA H", 2, 2, 8, shift<Reg8::H, Direction::Left, Shift::Arithmatic> };
    instr[BitShift::ShiftLeftArithRegL]
        = Instruction { "SLA L", 2, 2, 8, shift<Reg8::L, Direction::Left, Shift::Arithmatic> };
    instr[BitShift::ShiftLeftArithRegA]
        = Instruction { "SLA A", 2, 2, 8, shift<Reg8::A, Direction::Left, Shift::Arithmatic> };
    instr[BitShift::ShiftLeftArithIndirHL] = Instruction { "SLA [HL]", 2, 4, 16,
        shift<Reg8::IndirHL, Direction::Left, Shift::Arithmatic> };
    instr[BitShift::ShiftRightArithRegB]
        = Instruction { "SRA B", 2, 2, 8, shift<Reg8::B, Direction::Right, Shift::Arithmatic> };
    instr[BitShift::ShiftRightArithRegC]
        = Instruction { "SRA C", 2, 2, 8, shift<Reg8::C, Direction::Right, Shift::Arithmatic> };
    instr[BitShift::ShiftRightArithRegD]
        = Instruction { "SRA D", 2, 2, 8, shift<Reg8::D, Direction::Right, Shift::Arithmatic> };
    instr[BitShift::ShiftRightArithRegE]
        = Instruction { "SRA E", 2, 2, 8, shift<Reg8::E, Direction::Right, Shift::Arithmatic> };
    instr[BitShift::ShiftRightArithRegH]
        = Instruction { "SRA H", 2, 2, 8, shift<Reg8::H, Direction::Right, Shift::Arithmatic> };
    instr[BitShift::ShiftRightArithRegL]
        = Instruction { "SRA L", 2, 2, 8, shift<Reg8::L, Direction::Right, Shift::Arithmatic> };
    instr[BitShift::ShiftRightArithRegA]
        = Instruction { "SRA A", 2, 2, 8, shift<Reg8::A, Direction::Right, Shift::Arithmatic> };
    instr[BitShift::ShiftRightArithIndirHL] = Instruction { "SRA [HL]", 2, 4, 16,
        shift<Reg8::IndirHL, Direction::Right, Shift::Arithmatic> };
    instr[BitShift::ShiftRightLogicRegB]
        = Instruction { "SRL B", 2, 2, 8, shift<Reg8::B, Direction::Right, Shift::Logical> };
    instr[BitShift::ShiftRightLogicRegC]
        = Instruction { "SRL C", 2, 2, 8, shift<Reg8::C, Direction::Right, Shift::Logical> };
    instr[BitShift::ShiftRightLogicRegD]
        = Instruction { "SRL D", 2, 2, 8, shift<Reg8::D, Direction::Right, Shift::Logical> };
    instr[BitShift::ShiftRightLogicRegE]
        = Instruction { "SRL E", 2, 2, 8, shift<Reg8::E, Direction::Right, Shift::Logical> };
    instr[BitShift::ShiftRightLogicRegH]
        = Instruction { "SRL H", 2, 2, 8, shift<Reg8::H, Direction::Right, Shift::Logical> };
    instr[BitShift::ShiftRightLogicRegL]
        = Instruction { "SRL L", 2, 2, 8, shift<Reg8::L, Direction::Right, Shift::Logical> };
    instr[BitShift::ShiftRightLogicRegA]
        = Instruction { "SRL A", 2, 2, 8, shift<Reg8::A, Direction::Right, Shift::Logical> };
    instr[BitShift::ShiftRightLogicIndirHL] = Instruction { "SRL [HL]", 2, 4, 16,
        shift<Reg8::IndirHL, Direction::Right, Shift::Logical> };
    instr[BitShift::SwapRegB] = Instruction { "SWAP B", 2, 2, 8, swap<Reg8::B> };
    instr[BitShift::SwapRegC] = Instruction { "SWAP C", 2, 2, 8, swap<Reg8::C> };
    instr[BitShift::SwapRegD] = Instruction { "SWAP D", 2, 2, 8, swap<Reg8::D> };
    instr[BitShift::SwapRegE] = Instruction { "SWAP E", 2, 2, 8, swap<Reg8::E> };
    instr[BitShift::SwapRegH] = Instruction { "SWAP H", 2, 2, 8, swap<Reg8::H> };
    instr[BitShift::SwapRegL] = Instruction { "SWAP L", 2, 2, 8, swap<Reg8::L> };
    instr[BitShift::SwapRegA] = Instruction { "SWAP A", 2, 2, 8, swap<Reg8::A> };
    instr[BitShift::SwapIndirHL] = Instruction { "SWAP [HL]", 2, 4, 16, swap<Reg8::IndirHL> };
    instr[BitFlag::Bit0RegB] = Instruction { "BIT 0, B", 2, 2, 8, test_bit<0, Reg8::B> };
    instr[BitFlag::Bit0RegC] = Instruction { "BIT 0, C", 2, 2, 8, test_bit<0, Reg8::C> };
    instr[BitFlag::Bit0RegD] = Instruction { "BIT 0, D", 2, 2, 8, test_bit<0, Reg8::D> };
    instr[BitFlag::Bit0RegE] = Instruction { "BIT 0, E", 2, 2, 8, test_bit<0, Reg8::E> };
    instr[BitFlag::Bit0RegH] = Instruction { "BIT 0, H", 2, 2, 8, test_bit<0, Reg8::H> };
    instr[BitFlag::Bit0RegL] = Instruction { "BIT 0, L", 2, 2, 8, test_bit<0, Reg8::L> };
    instr[BitFlag::Bit0RegA] = Instruction { "BIT 0, A", 2, 2, 8, test_bit<0, Reg8::A> };
    instr[BitFlag::Bit1RegB] = Instruction { "BIT 1, B", 2, 2, 8, test_bit<1, Reg8::B> };
    instr[BitFlag::Bit1RegC] = Instruction { "BIT 1, C", 2, 2, 8, test_bit<1, Reg8::C> };
    instr[BitFlag::Bit1RegD] = Instruction { "BIT 1, D", 2, 2, 8, test_bit<1, Reg8::D> };
    instr[BitFlag::Bit1RegE] = Instruction { "BIT 1, E", 2, 2, 8, test_bit<1, Reg8::E> };
    instr[BitFlag::Bit1RegH] = Instruction { "BIT 1, H", 2, 2, 8, test_bit<1, Reg8::H> };
    instr[BitFlag::Bit1RegL] = Instruction { "BIT 1, L", 2, 2, 8, test_bit<1, Reg8::L> };
    instr[BitFlag::Bit1RegA] = Instruction { "BIT 1, A", 2, 2, 8, test_bit<1, Reg8::A> };
    instr[BitFlag::Bit2RegB] = Instruction { "BIT 2, B", 2, 2, 8, test_bit<2, Reg8::B> };
    instr[BitFlag::Bit2RegC] = Instruction { "BIT 2, C", 2, 2, 8, test_bit<2, Reg8::C> };
    instr[BitFlag::Bit2RegD] = Instruction { "BIT 2, D", 2, 2, 8, test_bit<2, Reg8::D> };
    instr[BitFlag::Bit2RegE] = Instruction { "BIT 2, E", 2, 2, 8, test_bit<2, Reg8::E> };
    instr[BitFlag::Bit2RegH] = Instruction { "BIT 2, H", 2, 2, 8, test_bit<2, Reg8::H> };
    instr[BitFlag::Bit2RegL] = Instruction { "BIT 2, L", 2, 2, 8, test_bit<2, Reg8::L> };
    instr[BitFlag::Bit2RegA] = Instruction { "BIT 2, A", 2, 2, 8, test_bit<2, Reg8::A> };
    instr[BitFlag::Bit3RegB] = Instruction { "BIT 3, B", 2, 2, 8, test_bit<3, Reg8::B> };
    instr[BitFlag::Bit3RegC] = Instruction { "BIT 3, C", 2, 2, 8, test_bit<3, Reg8::C> };
    instr[BitFlag::Bit3RegD] = Instruction { "BIT 3, D", 2, 2, 8, test_bit<3, Reg8::D> };
    instr[BitFlag::Bit3RegE] = Instruction { "BIT 3, E", 2, 2, 8, test_bit<3, Reg8::E> };
    instr[BitFlag::Bit3RegH] = Instruction { "BIT 3, H", 2, 2, 8, test_bit<3, Reg8::H> };
    instr[BitFlag::Bit3RegL] = Instruction { "BIT 3, L", 2, 2, 8, test_bit<3, Reg8::L> };
    instr[BitFlag::Bit3RegA] = Instruction { "BIT 3, A", 2, 2, 8, test_bit<3, Reg8::A> };
    instr[BitFlag::Bit4RegB] = Instruction { "BIT 4, B", 2, 2, 8, test_bit<4, Reg8::B> };
    instr[BitFlag::Bit4RegC] = Instruction { "BIT 4, C", 2, 2, 8, test_bit<4, Reg8::C> };
    instr[BitFlag::Bit4RegD] = Instruction { "BIT 4, D", 2, 2, 8, test_bit<4, Reg8::D> };
    instr[BitFlag::Bit4RegE] = Instruction { "BIT 4, E", 2, 2, 8, test_bit<4, Reg8::E> };
    instr[BitFlag::Bit4RegH] = Instruction { "BIT 4, H", 2, 2, 8, test_bit<4, Reg8::H> };
    instr[BitFlag::Bit4RegL] = Instruction { "BIT 4, L", 2, 2, 8, test_bit<4, Reg8::L> };
    instr[BitFlag::Bit4RegA] = Instruction { "BIT 4, A", 2, 2, 8, test_bit<4, Reg8::A> };
    instr[BitFlag::Bit5RegB] = Instruction { "BIT 5, B", 2, 2, 8, test_bit<5, Reg8::B> };
    instr[BitFlag::Bit5RegC] = Instruction { "BIT 5, C", 2, 2, 8, test_bit<5, Reg8::C> };
    instr[BitFlag::Bit5RegD] = Instruction { "BIT 5, D", 2, 2, 8, test_bit<5, Reg8::D> };
    instr[BitFlag::Bit5RegE] = Instruction { "BIT 5, E", 2, 2, 8, test_bit<5, Reg8::E> };
    instr[BitFlag::Bit5RegH] = Instruction { "BIT 5, H", 2, 2, 8, test_bit<5, Reg8::H> };
    instr[BitFlag::Bit5RegL] = Instruction { "BIT 5, L", 2, 2, 8, test_bit<5, Reg8::L> };
    instr[BitFlag::Bit5RegA] = Instruction { "BIT 5, A", 2, 2, 8, test_bit<5, Reg8::A> };
    instr[BitFlag::Bit6RegB] = Instruction { "BIT 6, B", 2, 2, 8, test_bit<6, Reg8::B> };
    instr[BitFlag::Bit6RegC] = Instruction { "BIT 6, C", 2, 2, 8, test_bit<6, Reg8::C> };
    instr[BitFlag::Bit6RegD] = Instruction { "BIT 6, D", 2, 2, 8, test_bit<6, Reg8::D> };
    instr[BitFlag::Bit6RegE] = Instruction { "BIT 6, E", 2, 2, 8, test_bit<6, Reg8::E> };
    instr[BitFlag::Bit6RegH] = Instruction { "BIT 6, H", 2, 2, 8, test_bit<6, Reg8::H> };
    instr[BitFlag::Bit6RegL] = Instruction { "BIT 6, L", 2, 2, 8, test_bit<6, Reg8::L> };
    instr[BitFlag::Bit6RegA] = Instruction { "BIT 6, A", 2, 2, 8, test_bit<6, Reg8::A> };
    instr[BitFlag::Bit7RegB] = Instruction { "BIT 7, B", 2, 2, 8, test_bit<7, Reg8::B> };
    instr[BitFlag::Bit7RegC] = Instruction { "BIT 7, C", 2, 2, 8, test_bit<7, Reg8::C> };
    instr[BitFlag::Bit7RegD] = Instruction { "BIT 7, D", 2, 2, 8, test_bit<7, Reg8::D> };
    instr[BitFlag::Bit7RegE] = Instruction { "BIT 7, E", 2, 2, 8, test_bit<7, Reg8::E> };
    instr[BitFlag::Bit7RegH] = Instruction { "BIT 7, H", 2, 2, 8, test_bit<7, Reg8::H> };
    instr[BitFlag::Bit7RegL] = Instruction { "BIT 7, L", 2, 2, 8, test_bit<7, Reg8::L> };
    instr[BitFlag::Bit7RegA] = Instruction { "BIT 7, A", 2, 2, 8, test_bit<7, Reg8::A> };
    instr[BitFlag::Bit0IndirHL]
        = Instruction { "BIT 0, [HL]", 2, 3, 12, test_bit<0, Reg8::IndirHL> };
    instr[BitFlag::Bit1IndirHL]
        = Instruction { "BIT 1, [HL]", 2, 3, 12, test_bit<1, Reg8::IndirHL> };
    instr[BitFlag::Bit2IndirHL]
        = Instruction { "BIT 2, [HL]", 2, 3, 12, test_bit<2, Reg8::IndirHL> };
    instr[BitFlag::Bit3IndirHL]
        = Instruction { "BIT 3, [HL]", 2, 3, 12, test_bit<3, Reg8::IndirHL> };
    instr[BitFlag::Bit4IndirHL]
        = Instruction { "BIT 4, [HL]", 2, 3, 12, test_bit<4, Reg8::IndirHL> };
    instr[BitFlag::Bit5IndirHL]
        = Instruction { "BIT 5, [HL]", 2, 3, 12, test_bit<5, Reg8::IndirHL> };
    instr[BitFlag::Bit6IndirHL]
        = Instruction { "BIT 6, [HL]", 2, 3, 12, test_bit<6, Reg8::IndirHL> };
    instr[BitFlag::Bit7IndirHL]
        = Instruction { "BIT 7, [HL]", 2, 3, 12, test_bit<7, Reg8::IndirHL> };
    instr[BitFlag::Reset0RegB] = Instruction { "RES 0, B", 2, 2, 8, reset_bit<0, Reg8::B> };
    instr[BitFlag::Reset0RegC] = Instruction { "RES 0, C", 2, 2, 8, reset_bit<0, Reg8::C> };
    instr[BitFlag::Reset0RegD] = Instruction { "RES 0, D", 2, 2, 8, reset_bit<0, Reg8::D> };
    instr[BitFlag::Reset0RegE] = Instruction { "RES 0, E", 2, 2, 8, reset_bit<0, Reg8::E> };
    instr[BitFlag::Reset0RegH] = Instruction { "RES 0, H", 2, 2, 8, reset_bit<0, Reg8::H> };
    instr[BitFlag::Reset0RegL] = Instruction { "RES 0, L", 2, 2, 8, reset_bit<0, Reg8::L> };
    instr[BitFlag::Reset0RegA] = Instruction { "RES 0, A", 2, 2, 8, reset_bit<0, Reg8::A> };
    instr[BitFlag::Reset1RegB] = Instruction { "RES 1, B", 2, 2, 8, reset_bit<1, Reg8::B> };
    instr[BitFlag::Reset1RegC] = Instruction { "RES 1, C", 2, 2, 8, reset_bit<1, Reg8::C> };
    instr[BitFlag::Reset1RegD] = Instruction { "RES 1, D", 2, 2, 8, reset_bit<1, Reg8::D> };
    instr[BitFlag::Reset1RegE] = Instruction { "RES 1, E", 2, 2, 8, reset_bit<1, Reg8::E> };
    instr[BitFlag::Reset1RegH] = Instruction { "RES 1, H", 2, 2, 8, reset_bit<1, Reg8::H> };
    instr[BitFlag::Reset1RegL] = Instruction { "RES 1, L", 2, 2, 8, reset_bit<1, Reg8::L> };
    instr[BitFlag::Reset1RegA] = Instruction { "RES 1, A", 2, 2, 8, reset_bit<1, Reg8::A> };
    instr[BitFlag::Reset2RegB] = Instruction { "RES 2, B", 2, 2, 8, reset_bit<2, Reg8::B> };
    instr[BitFlag::Reset2RegC] = Instruction { "RES 2, C", 2, 2, 8, reset_bit<2, Reg8::C> };
    instr[BitFlag::Reset2RegD] = Instruction { "RES 2, D", 2, 2, 8, reset_bit<2, Reg8::D> };
    instr[BitFlag::Reset2RegE] = Instruction { "RES 2, E", 2, 2, 8, reset_bit<2, Reg8::E> };
    instr[BitFlag::Reset2RegH] = Instruction { "RES 2, H", 2, 2, 8, reset_bit<2, Reg8::H> };
    instr[BitFlag::Reset2RegL] = Instruction { "RES 2, L", 2, 2, 8, reset_bit<2, Reg8::L> };
    instr[BitFlag::Reset2RegA] = Instruction { "RES 2, A", 2, 2, 8, reset_bit<2, Reg8::A> };
    instr[BitFlag::Reset3RegB] = Instruction { "RES 3, B", 2, 2, 8, reset_bit<3, Reg8::B> };
    instr[BitFlag::Reset3RegC] = Instruction { "RES 3, C", 2, 2, 8, reset_bit<3, Reg8::C> };
    instr[BitFlag::Reset3RegD] = Instruction { "RES 3, D", 2, 2, 8, reset_bit<3, Reg8::D> };
    instr[BitFlag::Reset3RegE] = Instruction { "RES 3, E", 2, 2, 8, reset_bit<3, Reg8::E> };
    instr[BitFlag::Reset3RegH] = Instruction { "RES 3, H", 2, 2, 8, reset_bit<3, Reg8::H> };
    instr[BitFlag::Reset3RegL] = Instruction { "RES 3, L", 2, 2, 8, reset_bit<3, Reg8::L> };
    instr[BitFlag::Reset3RegA] = Instruction { "RES 3, A", 2, 2, 8, reset_bit<3, Reg8::A> };
    instr[BitFlag::Reset4RegB] = Instruction { "RES 4, B", 2, 2, 8, reset_bit<4, Reg8::B> };
    instr[BitFlag::Reset4RegC] = Instruction { "RES 4, C", 2, 2, 8, reset_bit<4, Reg8::C> };
    instr[BitFlag::Reset4RegD] = Instruction { "RES 4, D", 2, 2, 8, reset_bit<4, Reg8::D> };
    instr[BitFlag::Reset4RegE] = Instruction { "RES 4, E", 2, 2, 8, reset_bit<4, Reg8::E> };
    instr[BitFlag::Reset4RegH] = Instruction { "RES 4, H", 2, 2, 8, reset_bit<4, Reg8::H> };
    instr[BitFlag::Reset4RegL] = Instruction { "RES 4, L", 2, 2, 8, reset_bit<4, Reg8::L> };
    instr[BitFlag::Reset4RegA] = Instruction { "RES 4, A", 2, 2, 8, reset_bit<4, Reg8::A> };
    instr[BitFlag::Reset5RegB] = Instruction { "RES 5, B", 2, 2, 8, reset_bit<5, Reg8::B> };
    instr[BitFlag::Reset5RegC] = Instruction { "RES 5, C", 2, 2, 8, reset_bit<5, Reg8::C> };
    instr[BitFlag::Reset5RegD] = Instruction { "RES 5, D", 2, 2, 8, reset_bit<5, Reg8::D> };
    instr[BitFlag::Reset5RegE] = Instruction { "RES 5, E", 2, 2, 8, reset_bit<5, Reg8::E> };
    instr[BitFlag::Reset5RegH] = Instruction { "RES 5, H", 2, 2, 8, reset_bit<5, Reg8::H> };
    instr[BitFlag::Reset5RegL] = Instruction { "RES 5, L", 2, 2, 8, reset_bit<5, Reg8::L> };
    instr[BitFlag::Reset5RegA] = Instruction { "RES 5, A", 2, 2, 8, reset_bit<5, Reg8::A> };
    instr[BitFlag::Reset6RegB] = Instruction { "RES 6, B", 2, 2, 8, reset_bit<6, Reg8::B> };
    instr[BitFlag::Reset6RegC] = Instruction { "RES 6, C", 2, 2, 8, reset_bit<6, Reg8::C> };
    instr[BitFlag::Reset6RegD] = Instruction { "RES 6, D", 2, 2, 8, reset_bit<6, Reg8::D> };
    instr[BitFlag::Reset6RegE] = Instruction { "RES 6, E", 2, 2, 8, reset_bit<6, Reg8::E> };
    instr[BitFlag::Reset6RegH] = Instruction { "RES 6, H", 2, 2, 8, reset_bit<6, Reg8::H> };
    instr[BitFlag::Reset6RegL] = Instruction { "RES 6, L", 2, 2, 8, reset_bit<6, Reg8::L> };
    instr[BitFlag::Reset6RegA] = Instruction { "RES 6, A", 2, 2, 8, reset_bit<6, Reg8::A> };
    instr[BitFlag::Reset7RegB] = Instruction { "RES 7, B", 2, 2, 8, reset_bit<7, Reg8::B> };
    instr[BitFlag::Reset7RegC] = Instruction { "RES 7, C", 2, 2, 8, reset_bit<7, Reg8::C> };
    instr[BitFlag::Reset7RegD] = Instruction { "RES 7, D", 2, 2, 8, reset_bit<7, Reg8::D> };
    instr[BitFlag::Reset7RegE] = Instruction { "RES 7, E", 2, 2, 8, reset_bit<7, Reg8::E> };
    instr[BitFlag::Reset7RegH] = Instruction { "RES 7, H", 2, 2, 8, reset_bit<7, Reg8::H> };
    instr[BitFlag::Reset7RegL] = Instruction { "RES 7, L", 2, 2, 8, reset_bit<7, Reg8::L> };
    instr[BitFlag::Reset7RegA] = Instruction { "RES 7, A", 2, 2, 8, reset_bit<7, Reg8::A> };
    instr[BitFlag::Reset0IndirHL]
        = Instruction { "RES 0, [HL]", 2, 4, 16, reset_bit<0, Reg8::IndirHL> };
    instr[BitFlag::Reset1IndirHL]
        = Instruction { "RES 1, [HL]", 2, 4, 16, reset_bit<1, Reg8::IndirHL> };
    instr[BitFlag::Reset2IndirHL]
        = Instruction { "RES 2, [HL]", 2, 4, 16, reset_bit<2, Reg8::IndirHL> };
    instr[BitFlag::Reset3IndirHL]
        = Instruction { "RES 3, [HL]", 2, 4, 16, reset_bit<3, Reg8::IndirHL> };
    instr[BitFlag::Reset4IndirHL]
        = Instruction { "RES 4, [HL]", 2, 4, 16, reset_bit<4, Reg8::IndirHL> };
    instr[BitFlag::Reset5IndirHL]
        = Instruction { "RES 5, [HL]", 2, 4, 16, reset_bit<5, Reg8::IndirHL> };
    instr[BitFlag::Reset6IndirHL]
        = Instruction { "RES 6, [HL]", 2, 4, 16, reset_bit<6, Reg8::IndirHL> };
    instr[BitFlag::Reset7IndirHL]
        = Instruction { "RES 7, [HL]", 2, 4, 16, reset_bit<7, Reg8::IndirHL> };
    instr[BitFlag::Set0RegB] = Instruction { "SET 0, B", 2, 2, 8, set_bit<0, Reg8::B> };
    instr[BitFlag::Set0RegC] = Instruction { "SET 0, C", 2, 2, 8, set_bit<0, Reg8::C> };
    instr[BitFlag::Set0RegD] = Instruction { "SET 0, D", 2, 2, 8, set_bit<0, Reg8::D> };
    instr[BitFlag::Set0RegE] = Instruction { "SET 0, E", 2, 2, 8, set_bit<0, Reg8::E> };
    instr[BitFlag::Set0RegH] = Instruction { "SET 0, H", 2, 2, 8, set_bit<0, Reg8::H> };
    instr[BitFlag::Set0RegL] = Instruction { "SET 0, L", 2, 2, 8, set_bit<0, Reg8::L> };
    instr[BitFlag::Set0RegA] = Instruction { "SET 0, A", 2, 2, 8, set_bit<0, Reg8::A> };
    instr[BitFlag::Set1RegB] = Instruction { "SET 1, B", 2, 2, 8, set_bit<1, Reg8::B> };
    instr[BitFlag::Set1RegC] = Instruction { "SET 1, C", 2, 2, 8, set_bit<1, Reg8::C> };
    instr[BitFlag::Set1RegD] = Instruction { "SET 1, D", 2, 2, 8, set_bit<1, Reg8::D> };
    instr[BitFlag::Set1RegE] = Instruction { "SET 1, E", 2, 2, 8, set_bit<1, Reg8::E> };
    instr[BitFlag::Set1RegH] = Instruction { "SET 1, H", 2, 2, 8, set_bit<1, Reg8::H> };
    instr[BitFlag::Set1RegL] = Instruction { "SET 1, L", 2, 2, 8, set_bit<1, Reg8::L> };
    instr[BitFlag::Set1RegA] = Instruction { "SET 1, A", 2, 2, 8, set_bit<1, Reg8::A> };
    instr[BitFlag::Set2RegB] = Instruction { "SET 2, B", 2, 2, 8, set_bit<2, Reg8::B> };
    instr[BitFlag::Set2RegC] = Instruction { "SET 2, C", 2, 2, 8, set_bit<2, Reg8::C> };
    instr[BitFlag::Set2RegD] = Instruction { "SET 2, D", 2, 2, 8, set_bit<2, Reg8::D> };
    instr[BitFlag::Set2RegE] = Instruction { "SET 2, E", 2, 2, 8, set_bit<2, Reg8::E> };
    instr[BitFlag::Set2RegH] = Instruction { "SET 2, H", 2, 2, 8, set_bit<2, Reg8::H> };
    instr[BitFlag::Set2RegL] = Instruction { "SET 2, L", 2, 2, 8, set_bit<2, Reg8::L> };
    instr[BitFlag::Set2RegA] = Instruction { "SET 2, A", 2, 2, 8, set_bit<2, Reg8::A> };
    instr[BitFlag::Set3RegB] = Instruction { "SET 3, B", 2, 2, 8, set_bit<3, Reg8::B> };
    instr[BitFlag::Set3RegC] = Instruction { "SET 3, C", 2, 2, 8, set_bit<3, Reg8::C> };
    instr[BitFlag::Set3RegD] = Instruction { "SET 3, D", 2, 2, 8, set_bit<3, Reg8::D> };
    instr[BitFlag::Set3RegE] = Instruction { "SET 3, E", 2, 2, 8, set_bit<3, Reg8::E> };
    instr[BitFlag::Set3RegH] = Instruction { "SET 3, H", 2, 2, 8, set_bit<3, Reg8::H> };
    instr[BitFlag::Set3RegL] = Instruction { "SET 3, L", 2, 2, 8, set_bit<3, Reg8::L> };
    instr[BitFlag::Set3RegA] = Instruction { "SET 3, A", 2, 2, 8, set_bit<3, Reg8::A> };
    instr[BitFlag::Set4RegB] = Instruction { "SET 4, B", 2, 2, 8, set_bit<4, Reg8::B> };
    instr[BitFlag::Set4RegC] = Instruction { "SET 4, C", 2, 2, 8, set_bit<4, Reg8::C> };
    instr[BitFlag::Set4RegD] = Instruction { "SET 4, D", 2, 2, 8, set_bit<4, Reg8::D> };
    instr[BitFlag::Set4RegE] = Instruction { "SET 4, E", 2, 2, 8, set_bit<4, Reg8::E> };
    instr[BitFlag::Set4RegH] = Instruction { "SET 4, H", 2, 2, 8, set_bit<4, Reg8::H> };
    instr[BitFlag::Set4RegL] = Instruction { "SET 4, L", 2, 2, 8, set_bit<4, Reg8::L> };
    instr[BitFlag::Set4RegA] = Instruction { "SET 4, A", 2, 2, 8, set_bit<4, Reg8::A> };
    instr[BitFlag::Set5RegB] = Instruction { "SET 5, B", 2, 2, 8, set_bit<5, Reg8::B> };
    instr[BitFlag::Set5RegC] = Instruction { "SET 5, C", 2, 2, 8, set_bit<5, Reg8::C> };
    instr[BitFlag::Set5RegD] = Instruction { "SET 5, D", 2, 2, 8, set_bit<5, Reg8::D> };
    instr[BitFlag::Set5RegE] = Instruction { "SET 5, E", 2, 2, 8, set_bit<5, Reg8::E> };
    instr[BitFlag::Set5RegH] = Instruction { "SET 5, H", 2, 2, 8, set_bit<5, Reg8::H> };
    instr[BitFlag::Set5RegL] = Instruction { "SET 5, L", 2, 2, 8, set_bit<5, Reg8::L> };
    instr[BitFlag::Set5RegA] = Instruction { "SET 5, A", 2, 2, 8, set_bit<5, Reg8::A> };
    instr[BitFlag::Set6RegB] = Instruction { "SET 6, B", 2, 2, 8, set_bit<6, Reg8::B> };
    instr[BitFlag::Set6RegC] = Instruction { "SET 6, C", 2, 2, 8, set_bit<6, Reg8::C> };
    instr[BitFlag::Set6RegD] = Instruction { "SET 6, D", 2, 2, 8, set_bit<6, Reg8::D> };
    instr[BitFlag::Set6RegE] = Instruction { "SET 6, E", 2, 2, 8, set_bit<6, Reg8::E> };
    instr[BitFlag::Set6RegH] = Instruction { "SET 6, H", 2, 2, 8, set_bit<6, Reg8::H> };
    instr[BitFlag::Set6RegL] = Instruction { "SET 6, L", 2, 2, 8, set_bit<6, Reg8::L> };
    instr[BitFlag::Set6RegA] = Instruction { "SET 6, A", 2, 2, 8, set_bit<6, Reg8::A> };
    instr[BitFlag::Set7RegB] = Instruction { "SET 7, B", 2, 2, 8, set_bit<7, Reg8::B> };
    instr[BitFlag::Set7RegC] = Instruction { "SET 7, C", 2, 2, 8, set_bit<7, Reg8::C> };
    instr[BitFlag::Set7RegD] = Instruction { "SET 7, D", 2, 2, 8, set_bit<7, Reg8::D> };
    instr[BitFlag::Set7RegE] = Instruction { "SET 7, E", 2, 2, 8, set_bit<7, Reg8::E> };
    instr[BitFlag::Set7RegH] = Instruction { "SET 7, H", 2, 2, 8, set_bit<7, Reg8::H> };
    instr[BitFlag::Set7RegL] = Instruction { "SET 7, L", 2, 2, 8, set_bit<7, Reg8::L> };
    instr[BitFlag::Set7RegA] = Instruction { "SET 7, A", 2, 2, 8, set_bit<7, Reg8::A> };
    instr[BitFlag::Set0IndirHL]
        = Instruction { "SET 0, [HL]", 2, 4, 16, set_bit<0, Reg8::IndirHL> };
    instr[BitFlag::Set1IndirHL]
        = Instruction { "SET 1, [HL]", 2, 4, 16, set_bit<1, Reg8::IndirHL> };
    instr[BitFlag::Set2IndirHL]
        = Instruction { "SET 2, [HL]", 2, 4, 16, set_bit<2, Reg8::IndirHL> };
    instr[BitFlag::Set3IndirHL]
        = Instruction { "SET 3, [HL]", 2, 4, 16, set_bit<3, Reg8::IndirHL> };
    instr[BitFlag::Set4IndirHL]
        = Instruction { "SET 4, [HL]", 2, 4, 16, set_bit<4, Reg8::IndirHL> };
    instr[BitFlag::Set5IndirHL]
        = Instruction { "SET 5, [HL]", 2, 4, 16, set_bit<5, Reg8::IndirHL> };
    instr[BitFlag::Set6IndirHL]
        = Instruction { "SET 6, [HL]", 2, 4, 16, set_bit<6, Reg8::IndirHL> };
    instr[BitFlag::Set7IndirHL]
        = Instruction { "SET 7, [HL]", 2, 4, 16, set_bit<7, Reg8::IndirHL> };
    return instr;
}

Sm83State::Sm83State(MemoryBus& memory)
    : regs { 0x01, 0x80, 0x00, 0x13, 0x00, 0xD8, 0x01, 0x4D }
    , mcycles(0)
    , tstates(0)
    , bus(memory)
    , mode(Sm83Mode::Running)
    , sp(0xFFFE)
    , pc(0x0100)
    , ime(true)
{
}

Sm83::Sm83(std::shared_ptr<spdlog::logger> log, MemoryBus& memory)
    : m_no_prefix_instr { new_no_prefix_instr() }
    , m_cb_prefix_instr { new_cb_prefix_instr() }
    , m_state(memory)
    , m_log(log)
{
}

void
Sm83::step()
{
    uint8_t opcode = m_state.bus.read_byte(m_state.pc++);
    Instruction instr = {};

    if (opcode == Misc::Prefix) {
        opcode = m_state.bus.read_byte(m_state.pc++);
        instr = m_cb_prefix_instr[opcode];
        if (!instr.execute) {
            throw IllegalOpcode(
                fmt::format("Illegal opcode {0} (0xCB 0x{1:02X})", instr.mnemonic, opcode));
        }
    } else {
        instr = m_no_prefix_instr[opcode];
        if (!instr.execute) {
            throw IllegalOpcode(
                fmt::format("Illegal opcode {0} (0x{1:02X})", instr.mnemonic, opcode));
        }
    }

    m_log->debug("Execute {0} ({1} bytes)", instr.mnemonic, instr.length);
    instr.execute(m_state);
    m_state.mcycles += instr.mcycles;
    m_state.tstates += instr.tstates;
}

size_t
Sm83::mcycles() const
{
    return m_state.mcycles;
}

size_t
Sm83::tstates() const
{
    return m_state.tstates;
}

IllegalOpcode::IllegalOpcode(std::string message)
    : m_message(message)
{
}

const char*
IllegalOpcode::what() const noexcept
{
    return m_message.c_str();
}
} // namespace cocoa::gb
