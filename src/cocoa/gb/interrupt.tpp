// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_GB_INTERRUPT_TPP
#define COCOA_GB_INTERRUPT_TPP

#include "cocoa/utility.hpp"

namespace cocoa::gb {
template <enum Interrupt Isr>
constexpr bool
is_interrupt_pending(const MemoryBus& bus)
{
    uint8_t ie_reg = bus.read_io_reg(IoMap::IE);
    uint8_t if_reg = bus.read_io_reg(IoMap::IF);
    return cocoa::is_bit_set<uint8_t, cocoa::from_enum(Isr)>(ie_reg)
        && cocoa::is_bit_set<uint8_t, cocoa::from_enum(Isr)>(if_reg);
}

template <enum Interrupt Isr>
constexpr void
request_interrupt(MemoryBus& bus)
{
    uint8_t if_reg = bus.read_io_reg(IoMap::IF);
    cocoa::set_bit<uint8_t, cocoa::from_enum(Isr)>(if_reg);
    bus.write_io_reg(IoMap::IF, if_reg);
}

template <enum Interrupt Isr>
constexpr void
clear_interrupt(MemoryBus& bus)
{
    uint8_t if_reg = bus.read_io_reg(IoMap::IF);
    cocoa::clear_bit<uint8_t, cocoa::from_enum(Isr)>(if_reg);
    bus.write_io_reg(IoMap::IF, if_reg);
}    
} // namespace cocoa::gb

#endif // COCOA_GB_INTERRUPT_TPP
