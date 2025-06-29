// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include <cstdint>

#include "cocoa/gb/memory.hpp"

namespace cocoa::gb {
uint8_t MemoryBus::read_u8(uint16_t address) const
{
    return m_bus[address];
}
} // namespace cocoa::gb
