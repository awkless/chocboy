// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include <cstdint>

#include "cocoa/gb/memory.hpp"
#include "cocoa/utility.hpp"

namespace cocoa::gb {
uint8_t
MemoryBus::read_byte(const uint16_t address) const
{
    return m_bus[address];
}

uint16_t
MemoryBus::read_word(const uint16_t address) const
{
    return from_pair(read_byte(address), read_byte(address + 1));
}

uint8_t
MemoryBus::read_io_reg(const IoMap reg) const
{
    return read_byte(from_enum(reg));
}

void
MemoryBus::write_byte(const uint16_t address, const uint8_t value)
{
    m_bus[address] = value;
}

void
MemoryBus::write_word(const uint16_t address, const uint16_t value)
{
    write_byte(address, from_high(value));
    write_byte(address + 1, from_low(value));
}

void
MemoryBus::write_io_reg(const IoMap reg, const uint8_t value)
{
    write_byte(from_enum(reg), value);
}

} // namespace cocoa::gb
