// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_GB_INTERRUPT_HPP
#define COCOA_GB_INTERRUPT_HPP

#include <cstdint>

#include "cocoa/gb/memory.hpp"

namespace cocoa::gb {
/// @brief Interrupt flags found in IF and IE registers.
enum class Interrupt {
    VBlank = 0,
    Lcd = 1,
    Timer = 2,
    Serial = 3,
    Joypad = 4,
};

/// @brief Check if target interrupt is pending.
///
/// Checks if interrupt flag is enabled in both IF and IE registers.
///
/// @param [in] bus Memory bus to check memory mapped IF and IE registers.
/// @return True if interrupt flag is set, false otherwise.
template <enum Interrupt Isr>
constexpr bool
is_interrupt_pending(const MemoryBus& bus);

/// @brief Request new interrupt.
///
/// Will set target flag in IF register.
///
/// @param [in] bus Memory bus to set interrupt flag in memory mapped IF register.
template <enum Interrupt Isr>
constexpr void
request_interrupt(MemoryBus& bus);

/// @brief Clear interrupt flag.
///
/// Will clear target flag in IF register.
///
/// @param [in] bus Memory bus to clear interrupt flag in memory mapped IF register.
template <enum Interrupt Isr>
constexpr void
clear_interrupt(MemoryBus& bus);
} // namespace cocoa::gb

#include "cocoa/gb/interrupt.tpp"

#endif // COCOA_GB_INTERRUPT_HPP
