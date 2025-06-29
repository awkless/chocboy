// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_GB_MEMORY_HPP
#define COCOA_GB_MEMORY_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace cocoa::gb {
/// @brief GameBoy memory map ranges.
///
/// @see https://gbdev.io/pandocs/Memory_Map.html
enum class MemoryMap : uint16_t {
    Rom0Start = 0x0000,
    Rom0End = 0x3FFF,
    RomXStart = 0x4000,
    RomXEnd = 0x7FFF,
    VramStart = 0x8000,
    VramEnd = 0x9FFF,
    SramStart = 0xA000,
    SramEnd = 0xBFFF,
    Wram0Start = 0xC000,
    Wram0End = 0xCFFF,
    WramXStart = 0xD000,
    WramXEnd = 0xDFFF,
    EchoRamStart = 0xE000,
    EchoRamEnd = 0xFDFF,
    OamStart = 0xFE00,
    OamEnd = 0xFE9F,
    UnusableAreaStart = 0xFEA0,
    UnusableAreaEnd = 0xFEFF,
    IoStart = 0xFF00,
    IoEnd = 0xFF7F,
    HramStart = 0xFF80,
    HramEnd = 0xFFFF,
};

constexpr size_t MEMORY_BUS_SIZE
    = std::numeric_limits<std::underlying_type<MemoryMap>::type>::max() + 1;

/// @brief GameBoy memory bus.
///
/// The GameBoy uses a 16-bit address bus with an 8-bit data bus, resulting in a 64 KiB memory bus.
/// The hardware employs memory mapping instead of port mapping to communicate with peripherals on
/// the SoC. Thus, this representation of the GameBoy memory bus is designed to be shared between
/// implementations of the GameBoy hardware for data transmission and communication with each other
/// much like how the original hardware does.
///
/// @see https://gbdev.io/pandocs/Memory_Map.html
class MemoryBus final {
public:
    MemoryBus() = default;

    ~MemoryBus() noexcept = default;

    [[nodiscard]]
    uint8_t read_u8(const uint16_t address) const;

    [[nodiscard]]
    uint16_t read_u16(const uint16_t address) const;

    void write_u8(const uint16_t address, const uint8_t value);

    void write_u16(const uint16_t address, const uint16_t value);

private:
    std::array<uint8_t, MEMORY_BUS_SIZE> m_bus;
};
} // namespace cocoa::gb

#endif // COOCA_GB_MEMORY_HPP
