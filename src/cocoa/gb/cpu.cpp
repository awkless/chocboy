// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#include "cocoa/gb/cpu.hpp"
#include "cocoa/gb/memory.hpp"

namespace cocoa::gb {
Sm83::Sm83(MemoryBus& bus)
    : m_sp(0)
    , m_pc(0)
    , m_cycles(0)
    , m_bus(bus)
{
}
} // namespace cocoa::gb
