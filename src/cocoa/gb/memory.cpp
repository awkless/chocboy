// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include <cstdint>

#include "cocoa/gb/memory.hpp"

namespace cocoa::gb {
uint8_t MemoryBus::read_u8(uint16_t address) const
{
    return m_bus[address];
}

uint16_t MemoryBus::read_u16(uint16_t address) const
{
    return static_cast<uint16_t>((m_bus[address] << 8) | m_bus[address + 1]);
}

void MemoryBus::write_u8(uint16_t address, uint8_t value)
{
    m_bus[address] = value;
}
} // namespace cocoa::gb
