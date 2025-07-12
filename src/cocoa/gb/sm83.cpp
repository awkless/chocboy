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
enum class Operation {
    Add,
    Sub,
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
load_reg8_reg8(Sm83State& cpu)
{
    cpu.reg.store_reg8<Dst>(cpu.reg.load_reg8<Src>());
}

template <enum Reg16 Dst, enum Reg16 Src>
static constexpr void
load_reg16_reg16(Sm83State& cpu)
{
    cpu.reg.store_reg16<Dst>(cpu.reg.load_reg16<Src>());
}

template <enum Reg8 Dst, enum Reg16 Src>
static constexpr void
load_reg8_reg16_indirect(Sm83State& cpu)
{
    cpu.reg.store_reg8<Dst>(cpu.reg.load_reg16_indirect<Src>(cpu.bus));
}

template <enum Reg8 Dst, enum Reg16 Src>
static constexpr void
load_reg8_reg16_inc_indirect(Sm83State& cpu)
{
    cpu.reg.store_reg8<Dst>(cpu.reg.load_reg16_inc_indirect<Src>(cpu.bus));
}

template <enum Reg8 Dst, enum Reg16 Src>
static constexpr void
load_reg8_reg16_dec_indirect(Sm83State& cpu)
{
    cpu.reg.store_reg8<Dst>(cpu.reg.load_reg16_dec_indirect<Src>(cpu.bus));
}

template <enum Reg16 Dst, enum Reg8 Src>
static constexpr void
load_reg16_indirect_reg8(Sm83State& cpu)
{
    cpu.reg.store_reg16_indirect<Dst>(cpu.bus, cpu.reg.load_reg8<Src>());
}

template <enum Reg16 Dst, enum Reg8 Src>
static constexpr void
load_reg16_inc_indirect_reg8(Sm83State& cpu)
{
    cpu.reg.store_reg16_inc_indirect<Dst>(cpu.bus, cpu.reg.load_reg8<Src>());
}

template <enum Reg16 Dst, enum Reg8 Src>
static constexpr void
load_reg16_dec_indirect_reg8(Sm83State& cpu)
{
    cpu.reg.store_reg16_dec_indirect<Dst>(cpu.bus, cpu.reg.load_reg8<Src>());
}

template <enum Reg8 Dst>
static constexpr void
load_reg8_imm8(Sm83State& cpu)
{
    cpu.reg.store_reg8<Dst>(cpu.load_imm8());
}

template <enum Reg16 Dst>
static constexpr void
load_reg16_indirect_imm8(Sm83State& cpu)
{
    cpu.reg.store_reg16_indirect<Dst>(cpu.bus, cpu.load_imm8());
}
template <enum Reg8 Dst>
static constexpr void
load_reg8_imm16_indirect(Sm83State& cpu)
{
    cpu.reg.store_reg8<Dst>(cpu.load_imm16_indirect());
}

template <enum Reg8 Src>
static constexpr void
load_imm16_indirect_reg8(Sm83State& cpu)
{
    cpu.store_imm16_indirect(cpu.reg.load_reg8<Src>());
}

template <enum Reg8 Dst, enum Reg8 Src>
static constexpr void
load_reg8_reg8_hram_indirect(Sm83State& cpu)
{
    cpu.reg.store_reg8<Dst>(cpu.reg.load_reg8_hram_indirect<Src>(cpu.bus));
}

template <enum Reg8 Dst, enum Reg8 Src>
static constexpr void
load_reg8_hram_indirect_reg8(Sm83State& cpu)
{
    cpu.reg.store_reg8_hram_indirect<Dst>(cpu.bus, cpu.reg.load_reg8<Src>());
}

template <enum Reg8 Dst>
static constexpr void
load_reg8_imm8_hram_indirect(Sm83State& cpu)
{
    cpu.reg.store_reg8<Dst>(cpu.load_imm8_hram_indirect());
}

template <enum Reg8 Src>
static constexpr void
load_imm8_hram_indirect_reg8(Sm83State& cpu)
{
    cpu.store_imm8_hram_indirect(cpu.reg.load_reg8<Src>());
}

template <enum Reg16 Dst>
static constexpr void
load_reg16_imm16(Sm83State& cpu)
{
    cpu.reg.store_reg16<Dst>(cpu.load_imm16());
}

template <enum Reg16 Src>
static constexpr void
load_imm16_indirect_reg16(Sm83State& cpu)
{
    cpu.store_imm16_indirect(cpu.reg.load_reg16<Src>());
}

template <enum Reg16 Src>
static constexpr void
push_reg16(Sm83State& cpu)
{
    uint16_t reg16 = cpu.reg.load_reg16<Src>();
    --cpu.reg.sp;
    cpu.bus.write_byte(cpu.reg.sp, cocoa::from_low(reg16));
    --cpu.reg.sp;
    cpu.bus.write_byte(cpu.reg.sp, cocoa::from_high(reg16));
}

template <enum Reg16 Dst>
static constexpr void
pop_reg16(Sm83State& cpu)
{
    uint8_t high = cpu.bus.read_byte(cpu.reg.sp);
    ++cpu.reg.sp;
    uint8_t low = cpu.bus.read_byte(cpu.reg.sp);
    ++cpu.reg.sp;
    cpu.reg.store_reg16<Dst>(cocoa::from_pair(high, low));
}

template <enum Reg16 Dst>
static constexpr void
load_reg16_stack_offset(Sm83State& cpu)
{
    int8_t offset = static_cast<int8_t>(cpu.load_imm8());
    uint16_t result = static_cast<uint16_t>(cpu.reg.sp + offset);
    cpu.reg.store_reg16<Dst>(result);
    cpu.reg.clear_flag<Flag::Z>();
    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(cpu.reg.sp, offset));
    cpu.reg.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, cpu.reg.sp));
}

enum class UseCarry {
    No,
    Yes,
};

static inline constexpr void
add_update_flags(
    Sm83State& cpu, const uint8_t result, const uint8_t operand1, const uint8_t operand2)
{
    cpu.reg.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(operand1, operand2));
    cpu.reg.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, operand1));
}

template <enum Reg8 Src, enum UseCarry C>
static constexpr void
add_reg8(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = 0;
    if constexpr (C == UseCarry::Yes)
        operand2 = cpu.reg.load_reg8<Src>() + cpu.reg.is_flag_set<Flag::C>();
    else
        operand2 = cpu.reg.load_reg8<Src>();

    uint8_t result = operand1 + operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    add_update_flags(cpu, result, operand1, operand2);
}

template <enum UseCarry C>
static constexpr void
add_imm8(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = 0;
    if constexpr (C == UseCarry::Yes)
        operand2 = cpu.load_imm8() + cpu.reg.is_flag_set<Flag::C>();
    else
        operand2 = cpu.load_imm8();

    uint8_t result = operand1 + operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    add_update_flags(cpu, result, operand1, operand2);
}

template <enum UseCarry C>
static constexpr void
add_reg16_hl_indirect(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = 0;
    if constexpr (C == UseCarry::Yes)
        operand2 = cpu.reg.load_reg16_indirect<Reg16::HL>(cpu.bus) + cpu.reg.is_flag_set<Flag::C>();
    else
        operand2 = cpu.reg.load_reg16_indirect<Reg16::HL>(cpu.bus);

    uint8_t result = operand1 + operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    add_update_flags(cpu, result, operand1, operand2);
}

static inline constexpr void
sub_update_flags(
    Sm83State& cpu, const uint8_t result, const uint8_t operand1, const uint8_t operand2)
{
    cpu.reg.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.reg.set_flag<Flag::N>();
    cpu.reg.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Sub>(operand1, operand2));
    cpu.reg.conditional_flag_toggle<Flag::C>(is_carry<Operation::Sub>(result, operand1));
}

template <enum Reg8 Src, enum UseCarry C>
static constexpr void
sub_reg8(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = 0;
    if constexpr (C == UseCarry::Yes)
        operand2 = cpu.reg.load_reg8<Src>() - cpu.reg.is_flag_set<Flag::C>();
    else
        operand2 = cpu.reg.load_reg8<Src>();

    uint8_t result = operand1 - operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    sub_update_flags(cpu, result, operand1, operand2);
}

template <enum UseCarry C>
static constexpr void
sub_imm8(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = 0;
    if constexpr (C == UseCarry::Yes)
        operand2 = cpu.load_imm8() - cpu.reg.is_flag_set<Flag::C>();
    else
        operand2 = cpu.load_imm8();

    uint8_t result = operand1 - operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    sub_update_flags(cpu, result, operand1, operand2);
}

template <enum UseCarry C>
static constexpr void
sub_reg16_hl_indirect(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = 0;
    if constexpr (C == UseCarry::Yes)
        operand2 = cpu.reg.load_reg16_indirect<Reg16::HL>(cpu.bus) - cpu.reg.is_flag_set<Flag::C>();
    else
        operand2 = cpu.reg.load_reg16_indirect<Reg16::HL>(cpu.bus);

    uint8_t result = operand1 - operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    sub_update_flags(cpu, result, operand1, operand2);
}

static inline constexpr void
and_update_flags(Sm83State& cpu, const uint8_t result)
{
    cpu.reg.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.set_flag<Flag::H>();
    cpu.reg.clear_flag<Flag::C>();
}

template <enum Reg8 Src>
static constexpr void
and_reg8(Sm83State& cpu)
{
    uint8_t result = cpu.reg.load_reg8<Reg8::A>() & cpu.reg.load_reg8<Src>();
    cpu.reg.store_reg8<Reg8::A>(result);
    and_update_flags(cpu, result);
}

static void
and_imm8(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.load_imm8();
    uint8_t result = operand1 & operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    and_update_flags(cpu, result);
}

static void
and_reg16_hl_indirect(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.reg.load_reg16_indirect<Reg16::HL>(cpu.bus);
    uint8_t result = operand1 & operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    and_update_flags(cpu, result);
}

static inline constexpr void
or_xor_update_flags(Sm83State& cpu, const uint8_t result)
{
    cpu.reg.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.clear_flag<Flag::H>();
    cpu.reg.clear_flag<Flag::C>();
}

template <enum Reg8 Src>
static constexpr void
or_reg8(Sm83State& cpu)
{
    uint8_t result = cpu.reg.load_reg8<Reg8::A>() | cpu.reg.load_reg8<Src>();
    cpu.reg.store_reg8<Reg8::A>(result);
    or_xor_update_flags(cpu, result);
}

static void
or_imm8(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.load_imm8();
    uint8_t result = operand1 | operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    or_xor_update_flags(cpu, result);
}

static void
or_reg16_hl_indirect(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.bus.read_byte(cpu.reg.load_reg16_indirect<Reg16::HL>(cpu.bus));
    uint8_t result = operand1 | operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    or_xor_update_flags(cpu, result);
}

template <enum Reg8 Src>
static constexpr void
xor_reg8(Sm83State& cpu)
{
    uint8_t result = cpu.reg.load_reg8<Reg8::A>() ^ cpu.reg.load_reg8<Src>();
    cpu.reg.store_reg8<Reg8::A>(result);
    or_xor_update_flags(cpu, result);
}

static void
xor_imm8(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.load_imm8();
    uint8_t result = operand1 ^ operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    or_xor_update_flags(cpu, result);
}

static void
xor_reg16_hl_indirect(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.reg.load_reg16_indirect<Reg16::HL>(cpu.bus);
    uint8_t result = operand1 ^ operand2;
    cpu.reg.store_reg8<Reg8::A>(result);
    or_xor_update_flags(cpu, result);
}

template <enum Reg8 Src>
static void
cp_reg8(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.reg.load_reg8<Src>();
    uint8_t result = operand1 - operand2;
    sub_update_flags(cpu, result, operand1, operand2);
}

static void
cp_imm8(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.load_imm8();
    uint8_t result = operand1 - operand2;
    sub_update_flags(cpu, result, operand1, operand2);
}

static void
cp_reg16_hl_indirect(Sm83State& cpu)
{
    uint8_t operand1 = cpu.reg.load_reg8<Reg8::A>();
    uint8_t operand2 = cpu.reg.load_reg16_indirect<Reg16::HL>(cpu.bus);
    uint8_t result = operand1 - operand2;
    sub_update_flags(cpu, result, operand1, operand2);
}

static inline constexpr void
inc_update_flags(Sm83State& cpu, const uint8_t result, const uint8_t operand1)
{
    cpu.reg.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(operand1, 1));
}

template <enum Reg8 Dst>
static constexpr void
inc_reg8(Sm83State& cpu)
{
    uint8_t operand = cpu.reg.load_reg8<Dst>();
    uint8_t result = operand + 1;
    cpu.reg.store_reg8<Dst>(result);
    inc_update_flags(cpu, result, operand);
}

static constexpr void
inc_reg16_hl_indirect(Sm83State& cpu)
{
    uint8_t operand = cpu.reg.load_reg16_indirect<Reg16::HL>(cpu.bus);
    uint8_t result = operand + 1;
    cpu.reg.store_reg16_indirect<Reg16::HL>(cpu.bus, result);
    inc_update_flags(cpu, result, operand);
}

static inline constexpr void
dec_update_flags(Sm83State& cpu, const uint8_t result, const uint8_t operand1)
{
    cpu.reg.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Sub>(operand1, 1));
}

template <enum Reg8 Dst>
static constexpr void
dec_reg8(Sm83State& cpu)
{
    uint8_t operand = cpu.reg.load_reg8<Dst>();
    uint8_t result = operand - 1;
    cpu.reg.store_reg8<Dst>(result);
    dec_update_flags(cpu, result, operand);
}

static constexpr void
dec_reg16_hl_indirect(Sm83State& cpu)
{
    uint8_t operand = cpu.reg.load_reg16_indirect<Reg16::HL>(cpu.bus);
    uint8_t result = operand - 1;
    cpu.reg.store_reg16_indirect<Reg16::HL>(cpu.bus, result);
    dec_update_flags(cpu, result, operand);
}

static void
complement_carry_flag(Sm83State& cpu)
{
    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.clear_flag<Flag::H>();
    cpu.reg.toggle_flag<Flag::C>();
}

static void
set_carry_flag(Sm83State& cpu)
{
    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.clear_flag<Flag::H>();
    cpu.reg.set_flag<Flag::C>();
}

static void
complement_reg8_a(Sm83State& cpu)
{
    cpu.reg.store_reg8<Reg8::A>(~cpu.reg.load_reg8<Reg8::A>());
    cpu.reg.set_flag<Flag::N>();
    cpu.reg.set_flag<Flag::H>();
}

static void
decimal_adjust_reg8_a(Sm83State& cpu)
{
    uint8_t rega = cpu.reg.load_reg8<Reg8::A>();
    if (!cpu.reg.is_flag_set<Flag::N>() || rega > 0x99) {
        rega += 0x60;
        cpu.reg.set_flag<Flag::C>();
    } else {
        if (cpu.reg.is_flag_set<Flag::C>())
            rega -= 0x60;
        if (cpu.reg.is_flag_set<Flag::H>())
            rega -= 0x06;
    }

    cpu.reg.store_reg8<Reg8::A>(rega);
    cpu.reg.conditional_flag_toggle<Flag::Z>(rega == 0);
    cpu.reg.clear_flag<Flag::H>();
}

template <enum Reg16 Src>
static constexpr void
add_reg16_hl_reg16(Sm83State& cpu)
{
    uint16_t operand1 = cpu.reg.load_reg16<Reg16::HL>();
    uint16_t operand2 = cpu.reg.load_reg16<Src>();
    uint16_t result = operand1 + operand2;
    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(operand1, operand2));
    cpu.reg.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, operand1));
}

static void
add_reg16_sp_offset(Sm83State& cpu)
{
    uint16_t operand1 = cpu.reg.sp;
    int8_t operand2 = static_cast<int8_t>(cpu.load_imm8());
    uint16_t result = static_cast<uint16_t>(operand1 + operand2);
    cpu.reg.sp = result;
    cpu.reg.clear_flag<Flag::Z>();
    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.conditional_flag_toggle<Flag::H>(is_half_carry<Operation::Add>(operand1, operand2));
    cpu.reg.conditional_flag_toggle<Flag::C>(is_carry<Operation::Add>(result, operand1));
}

static void
jump_imm16(Sm83State& cpu)
{
    cpu.reg.pc = cpu.load_imm16();
}

static void
jump_reg16_hl(Sm83State& cpu)
{
    cpu.reg.pc = cpu.reg.load_reg16<Reg16::HL>();
}

template <enum Condition C>
static constexpr void
jump_condition_imm16(Sm83State& cpu)
{
    uint16_t addr = cpu.load_imm16();
    if (cpu.reg.is_condition_set<C>()) {
        cpu.reg.pc = addr;
        cpu.mcycles += 1;
    }
}

static void
jump_relative_imm8(Sm83State& cpu)
{
    int8_t offset = static_cast<int8_t>(cpu.load_imm8());
    cpu.reg.pc = static_cast<uint8_t>(cpu.reg.pc + offset);
}

template <enum Condition C>
static constexpr void
jump_condition_relative_imm8(Sm83State& cpu)
{
    int8_t offset = static_cast<int8_t>(cpu.load_imm8());
    if (cpu.reg.is_condition_set<C>()) {
        cpu.reg.pc = static_cast<uint8_t>(cpu.reg.pc + offset);
        cpu.mcycles += 1;
    }
}

static void
call_imm16(Sm83State& cpu)
{
    uint16_t addr = cpu.load_imm16();
    --cpu.reg.sp;
    cpu.bus.write_byte(cpu.reg.sp, cocoa::from_low(cpu.reg.pc));
    --cpu.reg.sp;
    cpu.bus.write_byte(cpu.reg.sp, cocoa::from_high(cpu.reg.pc));
    cpu.reg.pc = addr;
}

template <enum Condition C>
static void
call_condition_imm16(Sm83State& cpu)
{
    uint16_t addr = cpu.load_imm16();
    if (cpu.reg.is_condition_set<C>()) {
        --cpu.reg.sp;
        cpu.bus.write_byte(cpu.reg.sp, cocoa::from_low(cpu.reg.pc));
        --cpu.reg.sp;
        cpu.bus.write_byte(cpu.reg.sp, cocoa::from_high(cpu.reg.pc));
        cpu.reg.pc = addr;
        cpu.mcycles += 3;
    }
}

static void
return_no_condition(Sm83State& cpu)
{
    uint8_t high = cpu.bus.read_byte(cpu.reg.sp);
    ++cpu.reg.sp;
    uint8_t low = cpu.bus.read_byte(cpu.reg.sp);
    ++cpu.reg.sp;
    cpu.reg.pc = cocoa::from_pair(high, low);
}

template <enum Condition C>
static void
return_condition(Sm83State& cpu)
{
    if (cpu.reg.is_condition_set<C>()) {
        uint8_t high = cpu.bus.read_byte(cpu.reg.sp);
        ++cpu.reg.sp;
        uint8_t low = cpu.bus.read_byte(cpu.reg.sp);
        ++cpu.reg.sp;
        cpu.reg.pc = cocoa::from_pair(high, low);
        cpu.mcycles += 3;
    }
}

static void
return_interrupt(Sm83State& cpu)
{
        uint8_t high = cpu.bus.read_byte(cpu.reg.sp);
        ++cpu.reg.sp;
        uint8_t low = cpu.bus.read_byte(cpu.reg.sp);
        ++cpu.reg.sp;
        cpu.reg.pc = cocoa::from_pair(high, low);
        cpu.ime = true;
}

template <uint8_t Vec>
static constexpr void
restart(Sm83State& cpu)
{
    --cpu.reg.sp;
    cpu.bus.write_byte(cpu.reg.sp, cocoa::from_low(cpu.reg.pc));
    --cpu.reg.sp;
    cpu.bus.write_byte(cpu.reg.sp, cocoa::from_high(cpu.reg.pc));
    cpu.reg.pc = cocoa::from_pair<uint16_t, uint8_t>(0x00, Vec);
}

template <enum Reg16 Dst>
static constexpr void
inc_reg16(Sm83State& cpu)
{
    cpu.reg.store_reg16<Dst>(cpu.reg.load_reg16<Dst>() + 1);
}

template <enum Reg16 Dst>
static constexpr void
dec_reg16(Sm83State& cpu)
{
    cpu.reg.store_reg16<Dst>(cpu.reg.load_reg16<Dst>() - 1);
}

static void
enable_interrupt(Sm83State& cpu)
{
    cpu.ime = true;
}

static void
halt(Sm83State& cpu)
{
    cpu.mode = ExecutionMode::Halted;
}

static void
stop(Sm83State& cpu)
{
    cpu.mode = ExecutionMode::Stopped;
}

static void
disable_interrupt(Sm83State& cpu)
{
    cpu.ime = false;
}

enum class Direction {
    Left,
    Right,
};

template <enum Direction D, enum Reg8 Dst>
static constexpr void
rotate(Sm83State& cpu)
{
    uint8_t carry = 0;
    uint8_t result = 0;

    if constexpr (D == Direction::Left) {
        carry = cpu.reg.load_reg8<Dst>() & 0x80 >> 8;
        result = static_cast<uint8_t>((cpu.reg.load_reg8<Dst>() << 1) | (cpu.reg.load_reg8<Dst>() >> 7));
    } else {
        carry = cpu.reg.load_reg8<Dst>() & 0x01;
        result = static_cast<uint8_t>((cpu.reg.load_reg8<Dst>() >> 1) | (cpu.reg.load_reg8<Dst>() << 7));
    }

    if constexpr (Dst == Reg8::A) {
        cpu.reg.clear_flag<Flag::Z>();
    } else {
        cpu.reg.conditional_flag_toggle<Flag::Z>(result == 0);
    }

    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.clear_flag<Flag::H>();
    cpu.reg.conditional_flag_toggle<Flag::C>(carry == 1);
}

template <enum Direction D, enum Reg8 Dst>
static constexpr void
rotate_carry(Sm83State& cpu)
{
    uint8_t carry = 0;
    uint8_t result = 0;

    if constexpr (D == Direction::Left) {
        carry = cpu.reg.load_reg8<Dst>() & 0x80 >> 8;
        result = static_cast<uint8_t>(cpu.reg.load_reg8<Dst>() << 1 | cpu.reg.is_flag_set<Flag::C>());
    } else {
        carry = cpu.reg.load_reg8<Dst>() & 0x01;
        result = static_cast<uint8_t>((cpu.reg.load_reg8<Dst>() >> 1) | (cpu.reg.is_flag_set<Flag::C>() << 7));
    }

    if constexpr (Dst == Reg8::A) {
        cpu.reg.clear_flag<Flag::Z>();
    } else {
        cpu.reg.conditional_flag_toggle<Flag::Z>(result == 0);
    }

    cpu.reg.conditional_flag_toggle<Flag::Z>(result == 0);
    cpu.reg.clear_flag<Flag::N>();
    cpu.reg.clear_flag<Flag::H>();
    cpu.reg.conditional_flag_toggle<Flag::C>(carry == 1);
}

static void
nop(Sm83State& cpu)
{
    // Do nothing other than suppress warning about unused parameter.
    (void)cpu;
}

enum Opcode : uint8_t {
    LoadRegBRegB = 0x40,
    LoadRegBRegC = 0x41,
    LoadRegBRegD = 0x42,
    LoadRegBRegE = 0x43,
    LoadRegBRegH = 0x44,
    LoadRegBRegL = 0x45,
    LoadRegBRegA = 0x47,
    LoadRegCRegB = 0x48,
    LoadRegCRegC = 0x49,
    LoadRegCRegD = 0x4A,
    LoadRegCRegE = 0x4B,
    LoadRegCRegH = 0x4C,
    LoadRegCRegL = 0x4D,
    LoadRegCRegA = 0x4F,
    LoadRegDRegB = 0x50,
    LoadRegDRegC = 0x51,
    LoadRegDRegD = 0x52,
    LoadRegDRegE = 0x53,
    LoadRegDRegH = 0x54,
    LoadRegDRegL = 0x55,
    LoadRegDRegA = 0x57,
    LoadRegERegB = 0x58,
    LoadRegERegC = 0x59,
    LoadRegERegD = 0x5A,
    LoadRegERegE = 0x5B,
    LoadRegERegH = 0x5C,
    LoadRegERegL = 0x5D,
    LoadRegERegA = 0x5F,
    LoadRegHRegB = 0x60,
    LoadRegHRegC = 0x61,
    LoadRegHRegD = 0x62,
    LoadRegHRegE = 0x63,
    LoadRegHRegH = 0x64,
    LoadRegHRegL = 0x65,
    LoadRegHRegA = 0x67,
    LoadRegLRegB = 0x68,
    LoadRegLRegC = 0x69,
    LoadRegLRegD = 0x6A,
    LoadRegLRegE = 0x6B,
    LoadRegLRegH = 0x6C,
    LoadRegLRegL = 0x6D,
    LoadRegLRegA = 0x6F,
    LoadRegARegB = 0x78,
    LoadRegARegC = 0x79,
    LoadRegARegD = 0x7A,
    LoadRegARegE = 0x7B,
    LoadRegARegH = 0x7C,
    LoadRegARegL = 0x7D,
    LoadRegARegA = 0x7F,
    LoadRegBImm8 = 0x06,
    LoadRegCImm8 = 0x0E,
    LoadRegDImm8 = 0x16,
    LoadRegEImm8 = 0x1E,
    LoadRegHImm8 = 0x26,
    LoadRegLImm8 = 0x2E,
    LoadRegAImm8 = 0x3E,
    LoadRegBIndirHL = 0x46,
    LoadRegCIndirHL = 0x4E,
    LoadRegDIndirHL = 0x56,
    LoadRegEIndirHL = 0x5E,
    LoadRegHIndirHL = 0x66,
    LoadRegLIndirHL = 0x6E,
    LoadRegAIndirHL = 0x7E,
    LoadIndirHLRegB = 0x70,
    LoadIndirHLRegC = 0x71,
    LoadIndirHLRegD = 0x72,
    LoadIndirHLRegE = 0x73,
    LoadIndirHLRegH = 0x74,
    LoadIndirHLRegL = 0x75,
    LoadIndirHLRegA = 0x77,
    LoadIndirHLImm8 = 0x36,
    LoadIndirBCRegA = 0x02,
    LoadIndirDERegA = 0x12,
    LoadRegAIndirBC = 0x0A,
    LoadRegAIndirDE = 0x1A,
    LoadIndirHLIRegA = 0x22,
    LoadIndirHLDRegA = 0x32,
    LoadRegAIndirHLI = 0x2A,
    LoadRegAIndirHLD = 0x3A,
    LoadRegAIndirImm16 = 0xFA,
    LoadIndirImm16RegA = 0xEA,
    LoadRegAIndirRegCHram = 0xF2,
    LoadIndirRegCHramRegA = 0xE2,
    LoadRegAIndirImm8Hram = 0xF0,
    LoadIndirImm8HramRegA = 0xE0,
    LoadRegBCImm16 = 0x01,
    LoadRegDEImm16 = 0x11,
    LoadRegHLImm16 = 0x21,
    LoadRegSPImm16 = 0x31,
    LoadRegSPRegHL = 0xF9,
    LoadIndirImm16RegSP = 0x08,
    PushRegBC = 0xC5,
    PushRegDE = 0xD5,
    PushRegHL = 0xE5,
    PushRegAF = 0xF5,
    PopRegBC = 0xC1,
    PopRegDE = 0xD1,
    PopRegHL = 0xE1,
    PopRegAF = 0xF1,
    LoadRegHLRegSPOffset = 0xF8,
    AddRegB = 0x80,
    AddRegC = 0x81,
    AddRegD = 0x82,
    AddRegE = 0x83,
    AddRegH = 0x84,
    AddRegL = 0x85,
    AddRegA = 0x87,
    AddCarryRegB = 0x88,
    AddCarryRegC = 0x89,
    AddCarryRegD = 0x8A,
    AddCarryRegE = 0x8B,
    AddCarryRegH = 0x8C,
    AddCarryRegL = 0x8D,
    AddCarryRegA = 0x8F,
    SubRegB = 0x90,
    SubRegC = 0x91,
    SubRegD = 0x92,
    SubRegE = 0x93,
    SubRegH = 0x94,
    SubRegL = 0x95,
    SubRegA = 0x97,
    SubCarryRegB = 0x98,
    SubCarryRegC = 0x99,
    SubCarryRegD = 0x9A,
    SubCarryRegE = 0x9B,
    SubCarryRegH = 0x9C,
    SubCarryRegL = 0x9D,
    SubCarryRegA = 0x9F,
    AddIndirHL = 0x86,
    AddCarryIndirHL = 0x8E,
    SubIndirHL = 0x96,
    SubCarryIndirHL = 0x9E,
    AddImm8 = 0xC6,
    AddCarryImm8 = 0xCE,
    SubImm8 = 0xD6,
    SubCarryImm8 = 0xDE,
    AndRegB = 0xA0,
    AndRegC = 0xA1,
    AndRegD = 0xA2,
    AndRegE = 0xA3,
    AndRegH = 0xA4,
    AndRegL = 0xA5,
    AndRegA = 0xA7,
    AndIndirHL = 0xA6,
    AndImm8 = 0xE6,
    XorRegB = 0x88,
    XorRegC = 0x89,
    XorRegD = 0x8A,
    XorRegE = 0x8B,
    XorRegH = 0x8C,
    XorRegL = 0x8D,
    XorRegA = 0x8F,
    XorIndirHL = 0x8E,
    XorImm8 = 0xEE,
    OrRegB = 0xB0,
    OrRegC = 0xB1,
    OrRegD = 0xB2,
    OrRegE = 0xB3,
    OrRegH = 0xB4,
    OrRegL = 0xB5,
    OrRegA = 0xB7,
    OrIndirHL = 0xB6,
    OrImm8 = 0xF6,
    CpRegB = 0xB8,
    CpRegC = 0xB9,
    CpRegD = 0xBA,
    CpRegE = 0xBB,
    CpRegH = 0xBC,
    CpRegL = 0xBD,
    CpRegA = 0xBF,
    CpIndirHL = 0xBE,
    CpImm8 = 0xFE,
    IncRegB = 0x04,
    IncRegC = 0x0C,
    IncRegD = 0x14,
    IncRegE = 0x1C,
    IncRegH = 0x24,
    IncRegL = 0x2C,
    IncRegA = 0x3C,
    IncIndirHL = 0x34,
    IncRegBC = 0x03,
    IncRegDE = 0x13,
    IncRegHL = 0x23,
    IncRegSP = 0x33,
    DecRegBC = 0x0B,
    DecRegDE = 0x1B,
    DecRegHL = 0x2B,
    DecRegSP = 0x3B,
    DecRegB = 0x05,
    DecRegC = 0x0D,
    DecRegD = 0x15,
    DecRegE = 0x1D,
    DecRegH = 0x25,
    DecRegL = 0x2D,
    DecRegA = 0x3D,
    DecIndirHL = 0x35,
    AddRegHLRegBC = 0x09,
    AddRegHLRegDE = 0x19,
    AddRegHLRegHL = 0x29,
    AddRegHLRegSP = 0x39,
    AddRegSPOffset = 0xE8,
    ComplementCarry = 0x3F,
    SetCarry = 0x37,
    DecimalAdjustRegA = 0x27,
    ComplementRegA = 0x2F,
    JumpImm16 = 0xC3,
    JumpRegHL = 0xE9,
    JumpNZImm16 = 0xC2,
    JumpNCImm16 = 0xD2,
    JumpZImm16 = 0xCA,
    JumpCImm16 = 0xDA,
    JumpRelImm8 = 0x18,
    JumpRelNZImm8 = 0x20,
    JumpRelNCImm8 = 0x30,
    JumpRelZImm8 = 0x28,
    JumpRelCImm8 = 0x38,
    CallImm16 = 0xCD,
    CallNZImm16 = 0xC4,
    CallNCImm16 = 0xD4,
    CallZImm16 = 0xCC,
    CallCImm16 = 0xDC,
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
    Rlca = 0x07,
    Rrca = 0x0F,
    Rla = 0x17,
    Rra = 0x1F,
    Nop = 0x00,
    Halt = 0x76,
    Stop = 0x10,
    DisableIR = 0xF3, 
    EnableIR = 0xFB,
    CBPrefix = 0xCB,
    Illegal00 = 0xD3,
    Illegal01 = 0xE3,
    Illegal02 = 0xE4,
    Illegal03 = 0xF4,
    Illegal04 = 0xDB,
    Illegal05 = 0xEB,
    Illegal06 = 0xEC,
    Illegal07 = 0xFC,
    Illegal08 = 0xDD,
    Illegal09 = 0xED,
    Illegal10 = 0xFD,
};

constexpr std::array<Instruction, 256>
new_no_prefix_instr()
{
    std::array<Instruction, 256> instr = {};
    instr[Opcode::LoadRegBRegB] = Instruction { "LD B, B", 1, 1, load_reg8_reg8<Reg8::B, Reg8::B> };
    instr[Opcode::LoadRegBRegC] = Instruction { "LD B, C", 1, 1, load_reg8_reg8<Reg8::B, Reg8::C> };
    instr[Opcode::LoadRegBRegD] = Instruction { "LD B, D", 1, 1, load_reg8_reg8<Reg8::B, Reg8::D> };
    instr[Opcode::LoadRegBRegE] = Instruction { "LD B, E", 1, 1, load_reg8_reg8<Reg8::B, Reg8::E> };
    instr[Opcode::LoadRegBRegH] = Instruction { "LD B, H", 1, 1, load_reg8_reg8<Reg8::B, Reg8::H> };
    instr[Opcode::LoadRegBRegL] = Instruction { "LD B, L", 1, 1, load_reg8_reg8<Reg8::B, Reg8::L> };
    instr[Opcode::LoadRegBRegA] = Instruction { "LD B, A", 1, 1, load_reg8_reg8<Reg8::B, Reg8::A> };
    instr[Opcode::LoadRegCRegB] = Instruction { "LD C, B", 1, 1, load_reg8_reg8<Reg8::C, Reg8::B> };
    instr[Opcode::LoadRegCRegC] = Instruction { "LD C, C", 1, 1, load_reg8_reg8<Reg8::C, Reg8::C> };
    instr[Opcode::LoadRegCRegD] = Instruction { "LD C, D", 1, 1, load_reg8_reg8<Reg8::C, Reg8::D> };
    instr[Opcode::LoadRegCRegE] = Instruction { "LD C, E", 1, 1, load_reg8_reg8<Reg8::C, Reg8::E> };
    instr[Opcode::LoadRegCRegH] = Instruction { "LD C, H", 1, 1, load_reg8_reg8<Reg8::C, Reg8::H> };
    instr[Opcode::LoadRegCRegL] = Instruction { "LD C, L", 1, 1, load_reg8_reg8<Reg8::C, Reg8::L> };
    instr[Opcode::LoadRegCRegA] = Instruction { "LD C, A", 1, 1, load_reg8_reg8<Reg8::C, Reg8::A> };
    instr[Opcode::LoadRegDRegB] = Instruction { "LD D, B", 1, 1, load_reg8_reg8<Reg8::D, Reg8::B> };
    instr[Opcode::LoadRegDRegC] = Instruction { "LD D, C", 1, 1, load_reg8_reg8<Reg8::D, Reg8::C> };
    instr[Opcode::LoadRegDRegD] = Instruction { "LD D, D", 1, 1, load_reg8_reg8<Reg8::D, Reg8::D> };
    instr[Opcode::LoadRegDRegE] = Instruction { "LD D, E", 1, 1, load_reg8_reg8<Reg8::D, Reg8::E> };
    instr[Opcode::LoadRegDRegH] = Instruction { "LD D, H", 1, 1, load_reg8_reg8<Reg8::D, Reg8::H> };
    instr[Opcode::LoadRegDRegL] = Instruction { "LD D, L", 1, 1, load_reg8_reg8<Reg8::D, Reg8::L> };
    instr[Opcode::LoadRegDRegA] = Instruction { "LD D, A", 1, 1, load_reg8_reg8<Reg8::D, Reg8::A> };
    instr[Opcode::LoadRegERegB] = Instruction { "LD E, B", 1, 1, load_reg8_reg8<Reg8::E, Reg8::B> };
    instr[Opcode::LoadRegERegC] = Instruction { "LD E, C", 1, 1, load_reg8_reg8<Reg8::E, Reg8::C> };
    instr[Opcode::LoadRegERegD] = Instruction { "LD E, D", 1, 1, load_reg8_reg8<Reg8::E, Reg8::D> };
    instr[Opcode::LoadRegERegE] = Instruction { "LD E, E", 1, 1, load_reg8_reg8<Reg8::E, Reg8::E> };
    instr[Opcode::LoadRegERegH] = Instruction { "LD E, H", 1, 1, load_reg8_reg8<Reg8::E, Reg8::H> };
    instr[Opcode::LoadRegERegL] = Instruction { "LD E, L", 1, 1, load_reg8_reg8<Reg8::E, Reg8::L> };
    instr[Opcode::LoadRegERegA] = Instruction { "LD E, A", 1, 1, load_reg8_reg8<Reg8::E, Reg8::A> };
    instr[Opcode::LoadRegHRegB] = Instruction { "LD H, B", 1, 1, load_reg8_reg8<Reg8::H, Reg8::B> };
    instr[Opcode::LoadRegHRegC] = Instruction { "LD H, C", 1, 1, load_reg8_reg8<Reg8::H, Reg8::C> };
    instr[Opcode::LoadRegHRegD] = Instruction { "LD H, D", 1, 1, load_reg8_reg8<Reg8::H, Reg8::D> };
    instr[Opcode::LoadRegHRegE] = Instruction { "LD H, E", 1, 1, load_reg8_reg8<Reg8::H, Reg8::E> };
    instr[Opcode::LoadRegHRegH] = Instruction { "LD H, H", 1, 1, load_reg8_reg8<Reg8::H, Reg8::H> };
    instr[Opcode::LoadRegHRegL] = Instruction { "LD H, L", 1, 1, load_reg8_reg8<Reg8::H, Reg8::L> };
    instr[Opcode::LoadRegHRegA] = Instruction { "LD H, A", 1, 1, load_reg8_reg8<Reg8::H, Reg8::A> };
    instr[Opcode::LoadRegLRegB] = Instruction { "LD L, B", 1, 1, load_reg8_reg8<Reg8::L, Reg8::B> };
    instr[Opcode::LoadRegLRegC] = Instruction { "LD L, C", 1, 1, load_reg8_reg8<Reg8::L, Reg8::C> };
    instr[Opcode::LoadRegLRegD] = Instruction { "LD L, D", 1, 1, load_reg8_reg8<Reg8::L, Reg8::D> };
    instr[Opcode::LoadRegLRegE] = Instruction { "LD L, E", 1, 1, load_reg8_reg8<Reg8::L, Reg8::E> };
    instr[Opcode::LoadRegLRegH] = Instruction { "LD L, H", 1, 1, load_reg8_reg8<Reg8::L, Reg8::H> };
    instr[Opcode::LoadRegLRegL] = Instruction { "LD L, L", 1, 1, load_reg8_reg8<Reg8::L, Reg8::L> };
    instr[Opcode::LoadRegLRegA] = Instruction { "LD L, A", 1, 1, load_reg8_reg8<Reg8::L, Reg8::A> };
    instr[Opcode::LoadRegARegB] = Instruction { "LD A, B", 1, 1, load_reg8_reg8<Reg8::A, Reg8::B> };
    instr[Opcode::LoadRegARegC] = Instruction { "LD A, C", 1, 1, load_reg8_reg8<Reg8::A, Reg8::C> };
    instr[Opcode::LoadRegARegD] = Instruction { "LD A, D", 1, 1, load_reg8_reg8<Reg8::A, Reg8::D> };
    instr[Opcode::LoadRegARegE] = Instruction { "LD A, E", 1, 1, load_reg8_reg8<Reg8::A, Reg8::E> };
    instr[Opcode::LoadRegARegH] = Instruction { "LD A, H", 1, 1, load_reg8_reg8<Reg8::A, Reg8::H> };
    instr[Opcode::LoadRegARegL] = Instruction { "LD A, L", 1, 1, load_reg8_reg8<Reg8::A, Reg8::L> };
    instr[Opcode::LoadRegARegA] = Instruction { "LD A, A", 1, 1, load_reg8_reg8<Reg8::A, Reg8::A> };
    instr[Opcode::LoadRegBImm8] = Instruction { "LD B, n8", 2, 2, load_reg8_imm8<Reg8::B> };
    instr[Opcode::LoadRegCImm8] = Instruction { "LD C, n8", 2, 2, load_reg8_imm8<Reg8::C> };
    instr[Opcode::LoadRegDImm8] = Instruction { "LD D, n8", 2, 2, load_reg8_imm8<Reg8::D> };
    instr[Opcode::LoadRegEImm8] = Instruction { "LD E, n8", 2, 2, load_reg8_imm8<Reg8::E> };
    instr[Opcode::LoadRegHImm8] = Instruction { "LD H, n8", 2, 2, load_reg8_imm8<Reg8::H> };
    instr[Opcode::LoadRegLImm8] = Instruction { "LD L, n8", 2, 2, load_reg8_imm8<Reg8::L> };
    instr[Opcode::LoadRegAImm8] = Instruction { "LD A, n8", 2, 2, load_reg8_imm8<Reg8::A> };
    instr[Opcode::LoadRegBIndirHL]
        = Instruction { "LD B, [HL]", 2, 1, load_reg8_reg16_indirect<Reg8::B, Reg16::HL> };
    instr[Opcode::LoadRegCIndirHL]
        = Instruction { "LD C, [HL]", 2, 1, load_reg8_reg16_indirect<Reg8::C, Reg16::HL> };
    instr[Opcode::LoadRegDIndirHL]
        = Instruction { "LD D, [HL]", 2, 1, load_reg8_reg16_indirect<Reg8::D, Reg16::HL> };
    instr[Opcode::LoadRegEIndirHL]
        = Instruction { "LD E, [HL]", 2, 1, load_reg8_reg16_indirect<Reg8::E, Reg16::HL> };
    instr[Opcode::LoadRegHIndirHL]
        = Instruction { "LD H, [HL]", 2, 1, load_reg8_reg16_indirect<Reg8::H, Reg16::HL> };
    instr[Opcode::LoadRegLIndirHL]
        = Instruction { "LD L, [HL]", 2, 1, load_reg8_reg16_indirect<Reg8::L, Reg16::HL> };
    instr[Opcode::LoadRegAIndirHL]
        = Instruction { "LD A, [HL]", 2, 1, load_reg8_reg16_indirect<Reg8::A, Reg16::HL> };
    instr[Opcode::LoadIndirHLRegB]
        = Instruction { "LD [HL], B", 2, 1, load_reg16_indirect_reg8<Reg16::HL, Reg8::B> };
    instr[Opcode::LoadIndirHLRegC]
        = Instruction { "LD [HL], C", 2, 1, load_reg16_indirect_reg8<Reg16::HL, Reg8::C> };
    instr[Opcode::LoadIndirHLRegD]
        = Instruction { "LD [HL], D", 2, 1, load_reg16_indirect_reg8<Reg16::HL, Reg8::D> };
    instr[Opcode::LoadIndirHLRegE]
        = Instruction { "LD [HL], E", 2, 1, load_reg16_indirect_reg8<Reg16::HL, Reg8::E> };
    instr[Opcode::LoadIndirHLRegH]
        = Instruction { "LD [HL], H", 2, 1, load_reg16_indirect_reg8<Reg16::HL, Reg8::H> };
    instr[Opcode::LoadIndirHLRegL]
        = Instruction { "LD [HL], L", 2, 1, load_reg16_indirect_reg8<Reg16::HL, Reg8::L> };
    instr[Opcode::LoadIndirHLRegA]
        = Instruction { "LD [HL], A", 2, 1, load_reg16_indirect_reg8<Reg16::HL, Reg8::A> };
    instr[Opcode::LoadIndirHLImm8]
        = Instruction { "LD [HL], n8", 3, 2, load_reg16_indirect_imm8<Reg16::HL> };
    instr[Opcode::LoadIndirBCRegA]
        = Instruction { "LD [BC], A", 2, 1, load_reg16_indirect_reg8<Reg16::BC, Reg8::A> };
    instr[Opcode::LoadIndirDERegA]
        = Instruction { "LD [DE], A", 2, 1, load_reg16_indirect_reg8<Reg16::DE, Reg8::A> };
    instr[Opcode::LoadRegAIndirBC]
        = Instruction { "LD A, [BC]", 2, 1, load_reg8_reg16_indirect<Reg8::A, Reg16::BC> };
    instr[Opcode::LoadRegAIndirDE]
        = Instruction { "LD A, [DE]", 2, 1, load_reg8_reg16_indirect<Reg8::A, Reg16::BC> };
    instr[Opcode::LoadIndirHLIRegA]
        = Instruction { "LD [HL+], A", 2, 1, load_reg16_inc_indirect_reg8<Reg16::HL, Reg8::A> };
    instr[Opcode::LoadIndirHLDRegA]
        = Instruction { "LD [HL-], A", 2, 1, load_reg16_dec_indirect_reg8<Reg16::HL, Reg8::A> };
    instr[Opcode::LoadRegAIndirHLI]
        = Instruction { "LD A, [HL+]", 2, 1, load_reg8_reg16_inc_indirect<Reg8::A, Reg16::HL> };
    instr[Opcode::LoadRegAIndirHLD]
        = Instruction { "LD A, [HL-]", 2, 1, load_reg8_reg16_dec_indirect<Reg8::A, Reg16::HL> };
    instr[Opcode::LoadRegAIndirImm16]
        = Instruction { "LD A, [n16]", 4, 3, load_reg8_imm16_indirect<Reg8::A> };
    instr[Opcode::LoadIndirImm16RegA]
        = Instruction { "LD [n16], A", 4, 3, load_imm16_indirect_reg8<Reg8::A> };
    instr[Opcode::LoadRegAIndirRegCHram]
        = Instruction { "LDH A, [C]", 2, 1, load_reg8_reg8_hram_indirect<Reg8::A, Reg8::C> };
    instr[Opcode::LoadIndirRegCHramRegA]
        = Instruction { "LDH [C], A", 2, 1, load_reg8_hram_indirect_reg8<Reg8::C, Reg8::A> };
    instr[Opcode::LoadRegAIndirImm8Hram]
        = Instruction { "LDH A, [n8]", 3, 2, load_reg8_imm8_hram_indirect<Reg8::A> };
    instr[Opcode::LoadIndirImm8HramRegA]
        = Instruction { "LDH [n8], A", 3, 2, load_imm8_hram_indirect_reg8<Reg8::A> };
    instr[Opcode::LoadRegBCImm16] = Instruction { "LD BC, n16", 3, 3, load_reg16_imm16<Reg16::BC> };
    instr[Opcode::LoadRegDEImm16] = Instruction { "LD DE, n16", 3, 3, load_reg16_imm16<Reg16::DE> };
    instr[Opcode::LoadRegHLImm16] = Instruction { "LD HL, n16", 3, 3, load_reg16_imm16<Reg16::HL> };
    instr[Opcode::LoadRegSPImm16] = Instruction { "LD SP, n16", 3, 3, load_reg16_imm16<Reg16::SP> };
    instr[Opcode::LoadRegSPRegHL]
        = Instruction { "LD SP, HL", 2, 1, load_reg16_reg16<Reg16::SP, Reg16::HL> };
    instr[Opcode::LoadIndirImm16RegSP]
        = Instruction { "LD [n16], SP", 5, 3, load_imm16_indirect_reg16<Reg16::SP> };
    instr[Opcode::PushRegBC] = Instruction { "PUSH BC", 4, 1, push_reg16<Reg16::BC> };
    instr[Opcode::PushRegDE] = Instruction { "PUSH DE", 4, 1, push_reg16<Reg16::DE> };
    instr[Opcode::PushRegHL] = Instruction { "PUSH HL", 4, 1, push_reg16<Reg16::HL> };
    instr[Opcode::PushRegAF] = Instruction { "PUSH AF", 4, 1, push_reg16<Reg16::AF> };
    instr[Opcode::PopRegBC] = Instruction { "POP BC", 3, 1, pop_reg16<Reg16::BC> };
    instr[Opcode::PopRegDE] = Instruction { "POP DE", 3, 1, pop_reg16<Reg16::DE> };
    instr[Opcode::PopRegHL] = Instruction { "POP HL", 3, 1, pop_reg16<Reg16::HL> };
    instr[Opcode::PopRegAF] = Instruction { "POP AF", 3, 1, pop_reg16<Reg16::AF> };
    instr[Opcode::LoadRegHLRegSPOffset]
        = Instruction { "LD HL, SP + e8", 3, 2, load_reg16_stack_offset<Reg16::HL> };
    instr[Opcode::AddRegB] = Instruction { "ADD A, B", 1, 1, add_reg8<Reg8::B, UseCarry::No> };
    instr[Opcode::AddRegC] = Instruction { "ADD A, C", 1, 1, add_reg8<Reg8::C, UseCarry::No> };
    instr[Opcode::AddRegD] = Instruction { "ADD A, D", 1, 1, add_reg8<Reg8::D, UseCarry::No> };
    instr[Opcode::AddRegE] = Instruction { "ADD A, E", 1, 1, add_reg8<Reg8::E, UseCarry::No> };
    instr[Opcode::AddRegH] = Instruction { "ADD A, H", 1, 1, add_reg8<Reg8::H, UseCarry::No> };
    instr[Opcode::AddRegL] = Instruction { "ADD A, L", 1, 1, add_reg8<Reg8::L, UseCarry::No> };
    instr[Opcode::AddRegA] = Instruction { "ADD A, A", 1, 1, add_reg8<Reg8::A, UseCarry::No> };
    instr[Opcode::AddCarryRegB]
        = Instruction { "ADC A, B", 1, 1, add_reg8<Reg8::B, UseCarry::Yes> };
    instr[Opcode::AddCarryRegC]
        = Instruction { "ADC A, C", 1, 1, add_reg8<Reg8::C, UseCarry::Yes> };
    instr[Opcode::AddCarryRegD]
        = Instruction { "ADC A, D", 1, 1, add_reg8<Reg8::D, UseCarry::Yes> };
    instr[Opcode::AddCarryRegE]
        = Instruction { "ADC A, E", 1, 1, add_reg8<Reg8::E, UseCarry::Yes> };
    instr[Opcode::AddCarryRegH]
        = Instruction { "ADC A, H", 1, 1, add_reg8<Reg8::H, UseCarry::Yes> };
    instr[Opcode::AddCarryRegL]
        = Instruction { "ADC A, L", 1, 1, add_reg8<Reg8::L, UseCarry::Yes> };
    instr[Opcode::AddCarryRegA]
        = Instruction { "ADC A, A", 1, 1, add_reg8<Reg8::A, UseCarry::Yes> };
    instr[Opcode::SubRegB] = Instruction { "SUB A, B", 1, 1, sub_reg8<Reg8::B, UseCarry::No> };
    instr[Opcode::SubRegC] = Instruction { "SUB A, C", 1, 1, sub_reg8<Reg8::C, UseCarry::No> };
    instr[Opcode::SubRegD] = Instruction { "SUB A, D", 1, 1, sub_reg8<Reg8::D, UseCarry::No> };
    instr[Opcode::SubRegE] = Instruction { "SUB A, E", 1, 1, sub_reg8<Reg8::E, UseCarry::No> };
    instr[Opcode::SubRegH] = Instruction { "SUB A, H", 1, 1, sub_reg8<Reg8::H, UseCarry::No> };
    instr[Opcode::SubRegL] = Instruction { "SUB A, L", 1, 1, sub_reg8<Reg8::L, UseCarry::No> };
    instr[Opcode::SubRegA] = Instruction { "SUB A, A", 1, 1, sub_reg8<Reg8::A, UseCarry::No> };
    instr[Opcode::SubCarryRegB]
        = Instruction { "SBC A, B", 1, 1, sub_reg8<Reg8::B, UseCarry::Yes> };
    instr[Opcode::SubCarryRegC]
        = Instruction { "SBC A, C", 1, 1, sub_reg8<Reg8::C, UseCarry::Yes> };
    instr[Opcode::SubCarryRegD]
        = Instruction { "SBC A, D", 1, 1, sub_reg8<Reg8::D, UseCarry::Yes> };
    instr[Opcode::SubCarryRegE]
        = Instruction { "SBC A, E", 1, 1, sub_reg8<Reg8::E, UseCarry::Yes> };
    instr[Opcode::SubCarryRegH]
        = Instruction { "SBC A, H", 1, 1, sub_reg8<Reg8::H, UseCarry::Yes> };
    instr[Opcode::SubCarryRegL]
        = Instruction { "SBC A, L", 1, 1, sub_reg8<Reg8::L, UseCarry::Yes> };
    instr[Opcode::SubCarryRegA]
        = Instruction { "SBC A, A", 1, 1, sub_reg8<Reg8::A, UseCarry::Yes> };
    instr[Opcode::AddImm8] = Instruction { "ADD A, n8", 2, 2, add_imm8<UseCarry::No> };
    instr[Opcode::AddCarryImm8] = Instruction { "ADC, A, n8", 2, 2, add_imm8<UseCarry::Yes> };
    instr[Opcode::SubImm8] = Instruction { "SUB A, n8", 2, 2, sub_imm8<UseCarry::No> };
    instr[Opcode::SubCarryImm8] = Instruction { "SBC A, n8", 2, 2, sub_imm8<UseCarry::Yes> };
    instr[Opcode::AddIndirHL]
        = Instruction { "ADD A, [HL]", 2, 1, add_reg16_hl_indirect<UseCarry::No> };
    instr[Opcode::AddCarryIndirHL]
        = Instruction { "ADC A, [HL]", 2, 1, add_reg16_hl_indirect<UseCarry::Yes> };
    instr[Opcode::SubIndirHL]
        = Instruction { "SUB A, [HL]", 2, 1, sub_reg16_hl_indirect<UseCarry::No> };
    instr[Opcode::SubCarryIndirHL]
        = Instruction { "SBC A, [HL]", 2, 1, sub_reg16_hl_indirect<UseCarry::Yes> };
    instr[Opcode::AndRegB] = Instruction { "AND A, B", 1, 1, and_reg8<Reg8::B> };
    instr[Opcode::AndRegC] = Instruction { "AND A, C", 1, 1, and_reg8<Reg8::C> };
    instr[Opcode::AndRegD] = Instruction { "AND A, D", 1, 1, and_reg8<Reg8::D> };
    instr[Opcode::AndRegE] = Instruction { "AND A, E", 1, 1, and_reg8<Reg8::E> };
    instr[Opcode::AndRegH] = Instruction { "AND A, H", 1, 1, and_reg8<Reg8::H> };
    instr[Opcode::AndRegL] = Instruction { "AND A, L", 1, 1, and_reg8<Reg8::L> };
    instr[Opcode::AndRegA] = Instruction { "AND A, A", 1, 1, and_reg8<Reg8::A> };
    instr[Opcode::AndIndirHL] = Instruction { "AND A, [HL]", 2, 1, and_reg16_hl_indirect };
    instr[Opcode::AndImm8] = Instruction { "AND A, n8", 2, 2, and_imm8 };
    instr[Opcode::OrRegB] = Instruction { "OR A, B", 1, 1, or_reg8<Reg8::B> };
    instr[Opcode::OrRegC] = Instruction { "OR A, C", 1, 1, or_reg8<Reg8::C> };
    instr[Opcode::OrRegD] = Instruction { "OR A, D", 1, 1, or_reg8<Reg8::D> };
    instr[Opcode::OrRegE] = Instruction { "OR A, E", 1, 1, or_reg8<Reg8::E> };
    instr[Opcode::OrRegH] = Instruction { "OR A, H", 1, 1, or_reg8<Reg8::H> };
    instr[Opcode::OrRegL] = Instruction { "OR A, L", 1, 1, or_reg8<Reg8::L> };
    instr[Opcode::OrRegA] = Instruction { "OR A, A", 1, 1, or_reg8<Reg8::A> };
    instr[Opcode::OrImm8] = Instruction { "OR A, n8", 2, 2, or_imm8 };
    instr[Opcode::OrIndirHL] = Instruction { "OR A, [HL]", 2, 1, or_reg16_hl_indirect };
    instr[Opcode::XorRegB] = Instruction { "XOR A, B", 1, 1, xor_reg8<Reg8::B> };
    instr[Opcode::XorRegC] = Instruction { "XOR A, C", 1, 1, xor_reg8<Reg8::C> };
    instr[Opcode::XorRegD] = Instruction { "XOR A, D", 1, 1, xor_reg8<Reg8::D> };
    instr[Opcode::XorRegE] = Instruction { "XOR A, E", 1, 1, xor_reg8<Reg8::E> };
    instr[Opcode::XorRegH] = Instruction { "XOR A, H", 1, 1, xor_reg8<Reg8::H> };
    instr[Opcode::XorRegL] = Instruction { "XOR A, L", 1, 1, xor_reg8<Reg8::L> };
    instr[Opcode::XorRegA] = Instruction { "XOR A, A", 1, 1, xor_reg8<Reg8::A> };
    instr[Opcode::XorImm8] = Instruction { "XOR A, n8", 2, 2, xor_imm8 };
    instr[Opcode::XorIndirHL] = Instruction { "XOR A, [HL]", 2, 1, xor_reg16_hl_indirect };
    instr[Opcode::CpRegB] = Instruction { "CP A, B", 1, 1, cp_reg8<Reg8::B> };
    instr[Opcode::CpRegC] = Instruction { "CP A, C", 1, 1, cp_reg8<Reg8::C> };
    instr[Opcode::CpRegD] = Instruction { "CP A, D", 1, 1, cp_reg8<Reg8::D> };
    instr[Opcode::CpRegE] = Instruction { "CP A, E", 1, 1, cp_reg8<Reg8::E> };
    instr[Opcode::CpRegH] = Instruction { "CP A, H", 1, 1, cp_reg8<Reg8::H> };
    instr[Opcode::CpRegL] = Instruction { "CP A, L", 1, 1, cp_reg8<Reg8::L> };
    instr[Opcode::CpRegA] = Instruction { "CP A, A", 1, 1, cp_reg8<Reg8::A> };
    instr[Opcode::CpImm8] = Instruction { "CP A, n8", 2, 2, cp_imm8 };
    instr[Opcode::CpIndirHL] = Instruction { "CP A, [HL]", 2, 1, cp_reg16_hl_indirect };
    instr[Opcode::IncRegB] = Instruction { "INC B", 1, 1, inc_reg8<Reg8::B> };
    instr[Opcode::IncRegC] = Instruction { "INC C", 1, 1, inc_reg8<Reg8::C> };
    instr[Opcode::IncRegD] = Instruction { "INC D", 1, 1, inc_reg8<Reg8::D> };
    instr[Opcode::IncRegE] = Instruction { "INC E", 1, 1, inc_reg8<Reg8::E> };
    instr[Opcode::IncRegH] = Instruction { "INC H", 1, 1, inc_reg8<Reg8::H> };
    instr[Opcode::IncRegL] = Instruction { "INC L", 1, 1, inc_reg8<Reg8::L> };
    instr[Opcode::IncRegA] = Instruction { "INC A", 1, 1, inc_reg8<Reg8::A> };
    instr[Opcode::IncIndirHL] = Instruction { "INC [HL]", 3, 1, inc_reg16_hl_indirect };
    instr[Opcode::DecRegB] = Instruction { "DEC B", 1, 1, dec_reg8<Reg8::B> };
    instr[Opcode::DecRegC] = Instruction { "DEC C", 1, 1, dec_reg8<Reg8::C> };
    instr[Opcode::DecRegD] = Instruction { "DEC D", 1, 1, dec_reg8<Reg8::D> };
    instr[Opcode::DecRegE] = Instruction { "DEC E", 1, 1, dec_reg8<Reg8::E> };
    instr[Opcode::DecRegH] = Instruction { "DEC H", 1, 1, dec_reg8<Reg8::H> };
    instr[Opcode::DecRegL] = Instruction { "DEC L", 1, 1, dec_reg8<Reg8::L> };
    instr[Opcode::DecRegA] = Instruction { "DEC A", 1, 1, dec_reg8<Reg8::A> };
    instr[Opcode::DecIndirHL] = Instruction { "DEC [HL]", 3, 1, dec_reg16_hl_indirect };
    instr[Opcode::IncRegBC] = Instruction { "INC BC", 2, 1, inc_reg16<Reg16::BC> };
    instr[Opcode::IncRegDE] = Instruction { "INC DE", 2, 1, inc_reg16<Reg16::DE> };
    instr[Opcode::IncRegHL] = Instruction { "INC HL", 2, 1, inc_reg16<Reg16::HL> };
    instr[Opcode::IncRegSP] = Instruction { "INC SP", 2, 1, inc_reg16<Reg16::SP> };
    instr[Opcode::DecRegBC] = Instruction { "DEC BC", 2, 1, dec_reg16<Reg16::BC> };
    instr[Opcode::DecRegDE] = Instruction { "DEC DE", 2, 1, dec_reg16<Reg16::DE> };
    instr[Opcode::DecRegHL] = Instruction { "DEC HL", 2, 1, dec_reg16<Reg16::HL> };
    instr[Opcode::DecRegSP] = Instruction { "DEC SP", 2, 1, dec_reg16<Reg16::SP> };
    instr[Opcode::ComplementCarry] = Instruction { "CCF", 1, 1, complement_carry_flag };
    instr[Opcode::SetCarry] = Instruction { "SCF", 1, 1, set_carry_flag };
    instr[Opcode::DecimalAdjustRegA] = Instruction { "DAA", 1, 1, decimal_adjust_reg8_a };
    instr[Opcode::ComplementRegA] = Instruction { "CPL", 1, 1, complement_reg8_a };
    instr[Opcode::JumpImm16] = Instruction { "JP n16", 4, 3, jump_imm16 };
    instr[Opcode::JumpRegHL] = Instruction { "JP HL", 1, 1, jump_reg16_hl };
    instr[Opcode::JumpNZImm16] =
        Instruction { "JP NZ, n16", 3, 3, jump_condition_imm16<Condition::NZ> };
    instr[Opcode::JumpNCImm16] =
        Instruction { "JP NC, n16", 3, 3, jump_condition_imm16<Condition::NC> };
    instr[Opcode::JumpZImm16] =
        Instruction { "JP Z, n16", 3, 3, jump_condition_imm16<Condition::Z> };
    instr[Opcode::JumpCImm16] =
        Instruction { "JP C, n16", 3, 3, jump_condition_imm16<Condition::C> };
    instr[Opcode::JumpRelImm8] = Instruction { "JP e8", 3, 2, jump_relative_imm8 };
    instr[Opcode::JumpRelNZImm8] =
        Instruction { "JP NZ, e8", 2, 2, jump_condition_relative_imm8<Condition::NZ> };
    instr[Opcode::JumpRelNCImm8] =
        Instruction { "JP NC, e8", 2, 2, jump_condition_relative_imm8<Condition::NC> };
    instr[Opcode::JumpRelZImm8] =
        Instruction { "JP Z, e8", 2, 2, jump_condition_relative_imm8<Condition::Z> };
    instr[Opcode::JumpRelCImm8] =
        Instruction { "JP C, e8", 2, 2, jump_condition_relative_imm8<Condition::C> };
    instr[Opcode::CallImm16] = Instruction { "CALL n16", 6, 3, call_imm16 };
    instr[Opcode::CallNZImm16] =
        Instruction { "CALL NZ, n16", 3, 3, call_condition_imm16<Condition::NZ> };
    instr[Opcode::CallNCImm16] =
        Instruction { "CALL NC, n16", 3, 3, call_condition_imm16<Condition::NC> };
    instr[Opcode::CallZImm16] =
        Instruction { "CALL Z, n16", 3, 3, call_condition_imm16<Condition::Z> };
    instr[Opcode::CallCImm16] =
        Instruction { "CALL C, n16", 3, 3, call_condition_imm16<Condition::C> };
    instr[Opcode::Return] = Instruction { "RET", 4, 1, return_no_condition };
    instr[Opcode::ReturnNZ] = Instruction { "RET NZ", 2, 1, return_condition<Condition::NZ> };
    instr[Opcode::ReturnNC] = Instruction { "RET NC", 2, 1, return_condition<Condition::NC> };
    instr[Opcode::ReturnZ] = Instruction { "RET Z", 2, 1, return_condition<Condition::Z> };
    instr[Opcode::ReturnC] = Instruction { "RET C", 2, 1, return_condition<Condition::C> };
    instr[Opcode::ReturnIR] = Instruction { "RETI", 4, 1, return_interrupt };
    instr[Opcode::Restart00] = Instruction { "RST 00", 4, 1, restart<0x00> };
    instr[Opcode::Restart10] = Instruction { "RST 10", 4, 1, restart<0x10> };
    instr[Opcode::Restart20] = Instruction { "RST 20", 4, 1, restart<0x20> };
    instr[Opcode::Restart30] = Instruction { "RST 30", 4, 1, restart<0x30> };
    instr[Opcode::Restart08] = Instruction { "RST 08", 4, 1, restart<0x08> };
    instr[Opcode::Restart18] = Instruction { "RST 18", 4, 1, restart<0x18> };
    instr[Opcode::Restart28] = Instruction { "RST 28", 4, 1, restart<0x28> };
    instr[Opcode::Restart38] = Instruction { "RST 38", 4, 1, restart<0x38> };
    instr[Opcode::AddRegHLRegBC] = Instruction { "ADD HL, BC", 2, 1, add_reg16_hl_reg16<Reg16::BC> };
    instr[Opcode::AddRegHLRegDE] = Instruction { "ADD HL, DE", 2, 1, add_reg16_hl_reg16<Reg16::DE> };
    instr[Opcode::AddRegHLRegHL] = Instruction { "ADD HL, HL", 2, 1, add_reg16_hl_reg16<Reg16::HL> };
    instr[Opcode::AddRegHLRegSP] = Instruction { "ADD HL, SP", 2, 1, add_reg16_hl_reg16<Reg16::SP> };
    instr[Opcode::AddRegSPOffset] = Instruction { "ADD SP, e8", 4, 2, add_reg16_sp_offset };
    instr[Opcode::EnableIR] = Instruction { "EI", 1, 1, enable_interrupt };
    instr[Opcode::DisableIR] = Instruction { "DI", 1, 1, disable_interrupt };
    instr[Opcode::Nop] = Instruction { "NOP", 1, 1, nop };
    instr[Opcode::Halt] = Instruction { "HALT", 1, 1, halt };
    instr[Opcode::Stop] = Instruction { "STOP", 1, 1, stop };
    instr[Opcode::Rlca] = Instruction { "RLCA", 1, 1, rotate_carry<Direction::Left, Reg8::A> };
    instr[Opcode::Rrca] = Instruction { "RRCA", 1, 1, rotate_carry<Direction::Right, Reg8::A> };
    instr[Opcode::Rla] = Instruction { "RLA", 1, 1, rotate<Direction::Left, Reg8::A> };
    instr[Opcode::Rra] = Instruction { "RRA", 1, 1, rotate<Direction::Right, Reg8::A> };
    instr[Opcode::Illegal00] = Instruction { "???", 0, 1, nullptr };
    instr[Opcode::Illegal01] = Instruction { "???", 0, 1, nullptr };
    instr[Opcode::Illegal02] = Instruction { "???", 0, 1, nullptr };
    instr[Opcode::Illegal03] = Instruction { "???", 0, 1, nullptr };
    instr[Opcode::Illegal04] = Instruction { "???", 0, 1, nullptr };
    instr[Opcode::Illegal05] = Instruction { "???", 0, 1, nullptr };
    instr[Opcode::Illegal06] = Instruction { "???", 0, 1, nullptr };
    instr[Opcode::Illegal07] = Instruction { "???", 0, 1, nullptr };
    instr[Opcode::Illegal08] = Instruction { "???", 0, 1, nullptr };
    instr[Opcode::Illegal09] = Instruction { "???", 0, 1, nullptr };
    instr[Opcode::Illegal10] = Instruction { "???", 0, 1, nullptr };
    return instr;
}

constexpr std::array<Instruction, 256>
new_cb_prefix_instr()
{
    std::array<Instruction, 256> instr = {};
    return instr;
}

Sm83State::Sm83State(MemoryBus& memory)
    : mcycles(0)
    , bus(memory)
    , mode(ExecutionMode::Running)
{
}

uint8_t
Sm83State::load_imm8()
{
    return bus.read_byte(reg.pc++);
}

uint8_t
Sm83State::load_imm8_hram_indirect()
{
    return bus.read_byte(cocoa::from_pair<uint16_t, uint8_t>(0xFF, load_imm8()));
}

void
Sm83State::store_imm8_hram_indirect(const uint8_t value)
{
    bus.write_byte(cocoa::from_pair<uint16_t, uint8_t>(0xFF, load_imm8()), value);
}

uint16_t
Sm83State::load_imm16()
{
    uint16_t imm16 = bus.read_word(reg.pc);
    reg.pc += 2;
    return imm16;
}

uint8_t
Sm83State::load_imm16_indirect()
{
    return bus.read_byte(load_imm16());
}

void
Sm83State::store_imm16_indirect(const uint8_t value)
{
    bus.write_byte(load_imm16(), value);
}

void
Sm83State::store_imm16_indirect(const uint16_t value)
{
    bus.write_word(load_imm16(), value);
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
    uint8_t opcode = m_state.bus.read_byte(m_state.reg.pc++);
    Instruction instr = {};

    if (opcode == Opcode::CBPrefix) {
        opcode = m_state.bus.read_byte(m_state.reg.pc++);
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
}

size_t
Sm83::mcycles() const
{
    return m_state.mcycles;
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
