// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_GB_SM83_TPP
#define COCOA_GB_SM83_TPP

namespace cocoa::gb {
template <enum Reg8 R>
[[nodiscard]]
constexpr uint8_t
Sm83State::load_reg8() const
{
    if constexpr (R == Reg8::B)
        return regs[RegIndex::B];
    if constexpr (R == Reg8::C)
        return regs[RegIndex::C];
    if constexpr (R == Reg8::IndirHramC)
        return bus.read_byte(cocoa::from_pair<uint16_t, uint8_t>(0xFF, regs[RegIndex::C]));
    if constexpr (R == Reg8::D)
        return regs[RegIndex::D];
    if constexpr (R == Reg8::E)
        return regs[RegIndex::E];
    if constexpr (R == Reg8::H)
        return regs[RegIndex::H];
    if constexpr (R == Reg8::L)
        return regs[RegIndex::L];
    if constexpr (R == Reg8::IndirHL)
        return bus.read_byte(load_reg16<Reg16::HL>());
    if constexpr (R == Reg8::A)
        return regs[RegIndex::A];
}

template <enum Reg8 R>
constexpr void
Sm83State::store_reg8(const uint8_t value)
{
    if constexpr (R == Reg8::B)
        regs[RegIndex::B] = value;
    if constexpr (R == Reg8::C)
        regs[RegIndex::C] = value;
    if constexpr (R == Reg8::IndirHramC)
        bus.write_byte(cocoa::from_pair<uint16_t, uint8_t>(0xFF, regs[RegIndex::C]), value);
    if constexpr (R == Reg8::D)
        regs[RegIndex::D] = value;
    if constexpr (R == Reg8::E)
        regs[RegIndex::E] = value;
    if constexpr (R == Reg8::H)
        regs[RegIndex::H] = value;
    if constexpr (R == Reg8::L)
        regs[RegIndex::L] = value;
    if constexpr (R == Reg8::IndirHL)
        bus.write_byte(load_reg16<Reg16::HL>(), value);
    if constexpr (R == Reg8::A)
        regs[RegIndex::A] = value;
}

template <enum Reg16 R>
[[nodiscard]]
constexpr uint16_t
Sm83State::load_reg16() const
{
    if constexpr (R == Reg16::BC)
        return cocoa::from_pair(load_reg8<Reg8::B>(), load_reg8<Reg8::C>());
    if constexpr (R == Reg16::DE)
        return cocoa::from_pair(load_reg8<Reg8::D>(), load_reg8<Reg8::E>());
    if constexpr (R == Reg16::HL)
        return cocoa::from_pair(load_reg8<Reg8::H>(), load_reg8<Reg8::L>());
    if constexpr (R == Reg16::SP)
        return sp;
}

template <enum Reg16Stack R>
[[nodiscard]]
constexpr uint16_t
Sm83State::load_reg16_stack() const
{
    if constexpr (R == Reg16Stack::BC)
        return load_reg16<Reg16::BC>();
    if constexpr (R == Reg16Stack::DE)
        return load_reg16<Reg16::DE>();
    if constexpr (R == Reg16Stack::HL)
        return load_reg16<Reg16::HL>();
    if constexpr (R == Reg16Stack::AF)
        return cocoa::from_pair(regs[RegIndex::A], regs[RegIndex::F]);
}

template <enum Reg16Indir R>
[[nodiscard]]
constexpr uint8_t
Sm83State::load_reg16_indir()
{
    if constexpr (R == Reg16Indir::BC)
        return bus.read_byte(load_reg16<Reg16::BC>());
    if constexpr (R == Reg16Indir::DE)
        return bus.read_byte(load_reg16<Reg16::DE>());

    if constexpr (R == Reg16Indir::HLI) {
        uint16_t addr = load_reg16<Reg16::HL>();
        uint8_t value = bus.read_byte(addr);
        store_reg16<Reg16::HL>(addr + 1);
        return value;
    }

    if constexpr (R == Reg16Indir::HLD) {
        uint16_t addr = load_reg16<Reg16::HL>();
        uint8_t value = bus.read_byte(addr);
        store_reg16<Reg16::HL>(addr - 1);
        return value;
    }
}

template <enum Reg16 R>
constexpr void
Sm83State::store_reg16(const uint16_t value)
{
    if constexpr (R == Reg16::BC) {
        store_reg8<Reg8::B>(cocoa::from_high(value));
        store_reg8<Reg8::C>(cocoa::from_low(value));
    }

    if constexpr (R == Reg16::DE) {
        store_reg8<Reg8::D>(cocoa::from_high(value));
        store_reg8<Reg8::E>(cocoa::from_low(value));
    }

    if constexpr (R == Reg16::HL) {
        store_reg8<Reg8::H>(cocoa::from_high(value));
        store_reg8<Reg8::L>(cocoa::from_low(value));
    }

    if constexpr (R == Reg16::SP)
        sp = value;
}

template <enum Reg16Stack R>
constexpr void
Sm83State::store_reg16_stack(const uint16_t value)
{
    if constexpr (R == Reg16Stack::BC)
        store_reg16<Reg16::BC>(value);
    if constexpr (R == Reg16Stack::DE)
        store_reg16<Reg16::DE>(value);
    if constexpr (R == Reg16Stack::HL)
        store_reg16<Reg16::HL>(value);

    if constexpr (R == Reg16Stack::AF) {
        regs[RegIndex::A] = cocoa::from_high(value);
        regs[RegIndex::F] = cocoa::from_low(value);
    }
}

template <enum Reg16Indir R>
constexpr void
Sm83State::store_reg16_indir(const uint8_t value)
{
    if constexpr (R == Reg16Indir::BC)
        bus.write_byte(load_reg16<Reg16::BC>(), value);
    if constexpr (R == Reg16Indir::DE)
        bus.write_byte(load_reg16<Reg16::DE>(), value);

    if constexpr (R == Reg16Indir::HLI) {
        uint16_t addr = load_reg16<Reg16::HL>();
        bus.write_byte(addr, value);
        store_reg16<Reg16::HL>(addr + 1);
    }

    if constexpr (R == Reg16Indir::HLD) {
        uint16_t addr = load_reg16<Reg16::HL>();
        bus.write_byte(addr, value);
        store_reg16<Reg16::HL>(addr - 1);
    }
}

template <enum Imm8 I>
[[nodiscard]]
constexpr uint8_t
Sm83State::load_imm8()
{
    if constexpr (I == Imm8::Direct)
        return bus.read_byte(pc++);
    if constexpr (I == Imm8::IndirHram)
        return bus.read_byte(cocoa::from_pair<uint16_t, uint8_t>(0xFF, bus.read_byte(pc++)));

    if constexpr (I == Imm8::IndirAbsolute) {
        uint8_t value = bus.read_byte(bus.read_word(pc));
        pc += 2;
        return value;
    }
}

template <enum Imm8 I>
constexpr void
Sm83State::store_imm8(const uint8_t value)
{
    static_assert(I != Imm8::Direct, "Direct 8-bit addressing cannot write to memory bus");

    if constexpr (I == Imm8::IndirHram)
        bus.write_byte(cocoa::from_pair<uint16_t, uint8_t>(0xFF, bus.read_byte(pc++)), value);

    if constexpr (I == Imm8::IndirAbsolute) {
        bus.write_byte(bus.read_word(pc), value);
        pc += 2;
    }
}

template <enum Imm16 I>
constexpr uint16_t
Sm83State::load_imm16()
{
    static_assert(I != Imm16::IndirAbsolute, "Absolute 16-bit indirect load not used by SM83 ISA");
    if constexpr (I == Imm16::Direct) {
        uint16_t value = bus.read_word(pc);
        pc += 2;
        return value;
    }
}

template <enum Imm16 I>
constexpr void
Sm83State::store_imm16(const uint16_t value)
{
    static_assert(I != Imm16::Direct, "Direct 16-bit addressing cannot write to memory bus ");
    if constexpr (I == Imm16::IndirAbsolute) {
        uint16_t addr = bus.read_word(pc);
        pc += 2;
        bus.write_word(addr, value);
    }
}

template <enum Flag F>
constexpr void
Sm83State::set_flag()
{
    uint8_t flag = regs[RegIndex::F];
    set_bit<uint8_t, cocoa::from_enum(F)>(flag);
    regs[RegIndex::F] = flag;
}

template <enum Flag F>
constexpr void
Sm83State::clear_flag()
{
    uint8_t flag = regs[RegIndex::F];
    clear_bit<uint8_t, cocoa::from_enum(F)>(flag);
    regs[RegIndex::F] = flag;
}

template <enum Flag F>
constexpr void
Sm83State::conditional_flag_toggle(bool condition)
{
    uint8_t flag = regs[RegIndex::F];
    conditional_bit_toggle<uint8_t, cocoa::from_enum(F)>(flag, condition);
    regs[RegIndex::F] = flag;
}

template <enum Flag F>
constexpr void
Sm83State::toggle_flag()
{
    uint8_t flag = regs[RegIndex::F];
    toggle_bit<uint8_t, cocoa::from_enum(F)>(flag);
    regs[RegIndex::F] = flag;
}

template <enum Flag F>
[[nodiscard]]
constexpr bool
Sm83State::is_flag_set() const
{
    uint8_t flag = regs[RegIndex::F];
    return is_bit_set<uint8_t, cocoa::from_enum(F)>(flag);
}

template <enum Condition C>
[[nodiscard]]
constexpr bool
Sm83State::is_condition_set() const
{
    if constexpr (C == Condition::NZ)
        return !is_flag_set<Flag::Z>();
    if constexpr (C == Condition::Z)
        return is_flag_set<Flag::Z>();
    if constexpr (C == Condition::NC)
        return !is_flag_set<Flag::C>();
    if constexpr (C == Condition::C)
        return is_flag_set<Flag::C>();
}
} // namespace cocoa::gb

#endif // COCOA_GB_SM83_TPP
