// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include <cstdint>

#include "cocoa/gb/memory.hpp"
#include "cocoa/utilities.hpp"

namespace cocoa::gb {
uint8_t MemoryBus::read_u8(const uint16_t address) const
{
    return m_bus[address];
}

uint16_t MemoryBus::read_u16(const uint16_t address) const
{
    return from_pair(read_u8(address), read_u8(address + 1));
}

void MemoryBus::write_u8(const uint16_t address, const uint8_t value)
{
    m_bus[address] = value;
}

void MemoryBus::write_u16(const uint16_t address, const uint16_t value)
{
    write_u8(address, from_high(value));
    write_u8(address + 1, from_low(value));
}
} // namespace cocoa::gb
