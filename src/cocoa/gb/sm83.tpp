// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_GB_SM83_TPP
#define COCOA_GB_SM83_TPP

namespace cocoa::gb {
template <enum Reg8 R>
[[nodiscard]]
constexpr uint8_t RegisterFile::load_reg8() const
{
    return regs[cocoa::from_enum(R)];
}

template <enum Reg8 R>
[[nodiscard]]
constexpr uint8_t RegisterFile::load_reg8_hram_indirect(const MemoryBus& bus) const
{
    return bus.read_byte(cocoa::from_pair<uint16_t, uint8_t>(0xFF, load_reg8<R>()));
}

template <enum Reg8 R>
constexpr void RegisterFile::store_reg8(const uint8_t value)
{
    regs[cocoa::from_enum(R)] = value;
}

template <enum Reg8 R>
constexpr void RegisterFile::store_reg8_hram_indirect(MemoryBus& bus, const uint8_t value)
{
    bus.write_byte(cocoa::from_pair<uint16_t, uint8_t>(0xFF, load_reg8<R>()), value);
}

template <enum Reg16 R>
[[nodiscard]]
constexpr uint16_t RegisterFile::load_reg16() const
{
    if constexpr (R == Reg16::AF)
        return cocoa::from_pair(load_reg8<Reg8::A>(), load_reg8<Reg8::F>());
    if constexpr (R == Reg16::BC)
        return cocoa::from_pair(load_reg8<Reg8::B>(), load_reg8<Reg8::C>());
    if constexpr (R == Reg16::DE)
        return cocoa::from_pair(load_reg8<Reg8::D>(), load_reg8<Reg8::E>());
    if constexpr (R == Reg16::HL)
        return cocoa::from_pair(load_reg8<Reg8::H>(), load_reg8<Reg8::L>());
    if constexpr (R == Reg16::SP)
        return sp;
}

template <enum Reg16 R>
[[nodiscard]]
constexpr uint8_t RegisterFile::load_reg16_indirect(const MemoryBus& bus) const
{
    return bus.read_byte(load_reg16<R>());
}

template <enum Reg16 R>
[[nodiscard]]
constexpr uint8_t RegisterFile::load_reg16_inc_indirect(const MemoryBus& bus)
{
    uint16_t addr = load_reg16<R>();
    uint8_t value = bus.read_byte(addr);
    store_reg16<R>(addr + 1);
    return value;
}

template <enum Reg16 R>
[[nodiscard]]
constexpr uint8_t RegisterFile::load_reg16_dec_indirect(const MemoryBus& bus)
{
    uint16_t addr = load_reg16<R>();
    uint8_t value = bus.read_byte(addr);
    store_reg16<R>(addr - 1);
    return value;
}

template <enum Reg16 R>
constexpr void RegisterFile::store_reg16(const uint16_t value)
{
    if constexpr (R == Reg16::AF) {
        store_reg8<Reg8::A>(cocoa::from_high(value));
        store_reg8<Reg8::F>(cocoa::from_low(value));
    }

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

template <enum Reg16 R>
constexpr void RegisterFile::store_reg16_indirect(MemoryBus& bus, const uint8_t value)
{
    bus.write_byte(load_reg16<R>(), value);
}

template <enum Reg16 R>
constexpr void RegisterFile::store_reg16_inc_indirect(MemoryBus& bus, const uint8_t value)
{
    uint16_t addr = load_reg16<R>();
    bus.write_byte(addr++, value);
    store_reg16<R>(addr);
}

template <enum Reg16 R>
constexpr void RegisterFile::store_reg16_dec_indirect(MemoryBus& bus, const uint8_t value)
{
    uint16_t addr = load_reg16<R>();
    bus.write_byte(addr--, value);
    store_reg16<R>(addr);
}

template <enum Flag F>
constexpr void RegisterFile::set_flag()
{
    uint8_t flag = load_reg8<Reg8::F>();
    set_bit<uint8_t, cocoa::from_enum(F)>(flag);
    store_reg8<Reg8::F>(flag);
}

template <enum Flag F>
constexpr void RegisterFile::clear_flag()
{
    uint8_t flag = load_reg8<Reg8::F>();
    clear_bit<uint8_t, cocoa::from_enum(F)>(flag);
    store_reg8<Reg8::F>(flag);
}

template <enum Flag F>
constexpr void RegisterFile::conditional_flag_toggle(bool condition)
{
    uint8_t flag = load_reg8<Reg8::F>();
    conditional_bit_toggle<uint8_t, cocoa::from_enum(F)>(flag, condition);
    store_reg8<Reg8::F>(flag);
}

template <enum Flag F>
constexpr void RegisterFile::toggle_flag()
{
    uint8_t flag = load_reg8<Reg8::F>();
    toggle_bit<uint8_t, cocoa::from_enum(F)>(flag);
    store_reg8<Reg8::F>(flag);
}

template <enum Flag F>
[[nodiscard]]
constexpr bool RegisterFile::is_flag_set() const
{
    uint8_t flag = load_reg8<Reg8::F>();
    return is_bit_set<uint8_t, cocoa::from_enum(F)>(flag);
}

template <enum Condition C>
[[nodiscard]]
constexpr bool RegisterFile::is_condition_set() const
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
