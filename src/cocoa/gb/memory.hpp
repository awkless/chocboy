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
constexpr size_t MEMORY_BUS_SIZE = 65535;

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

/// @brief I/O register ranges from 0xFF00 to 0xFF7F.
///
/// @see https://gbdev.io/pandocs/Memory_Map.html
enum class IoMap : uint16_t {
    /// Joypad register.
    JOYP = 0xFF00,

    /// Serial transfer data register.
    SB = 0xFF01,

    /// Serial transfer control register.
    SC = 0xFF02,

    /// Divider register.
    DIV = 0xFF04,

    /// Timer counter register.
    TIMA = 0xFF05,

    /// Timer modulo register.
    TMA = 0xFF06,

    /// Timer control register.
    TAC = 0xFF07,

    /// Interrupt flag register.
    IF = 0xFF0F,

    /// Audio master control register.
    NR52 = 0xFF26,

    /// Sound panning register
    NR51 = 0xFF25,

    /// Masater volume and & VIM panning register.
    NR50 = 0xFF24,

    /// Channel 1 sweep register.
    NR10 = 0xFF10,

    /// Channel 1 length timer & duty cycle register.
    NR11 = 0xFF11,

    /// Channel 1 volume & envelope.
    NR12 = 0xFF12,

    /// Channel 1 period low (write-only).
    NR13 = 0xFF13,

    /// Channel 1 period high & control register.
    NR14 = 0xFF14,

    /// Channel 2 length timer & duty cycle register.
    NR21 = 0xFF16,

    /// Channel 2 volume & envelope.
    NR22 = 0xFF17,

    /// Channel 2 period low (write-only).
    NR23 = 0xFF18,

    /// Channel 2 period high & control register.
    NR24 = 0xFF19,

    /// Channel 3 DAC enable register.
    NR30 = 0xFF1A,

    /// Channel 3 length timer (write-only) register.
    NR31 = 0xFF1B,

    /// Channel 3 output level register.
    NR32 = 0xFF1C,

    /// Channel 3 period low (write-only) register.
    NR33 = 0xFF1D,

    /// Channel 3 period high & control register.
    NR34 = 0xFF1E,

    /// Wave pattern RAM.
    WavePatternRamStart = 0xFF30,
    WavePatternRamEnd = 0xFF3F,

    /// Channel 4 length timer (write-only) register.
    NR41 = 0xFF20,

    /// Channel 4 volume & envelope register.
    NR42 = 0xFF21,

    /// Channel 4 frequence & randomness register.
    NR43 = 0xFF22,

    /// Channel 4 control register.
    NR44 = 0xFF23,

    /// LCD Control register.
    LCDC = 0xFF40,

    /// LCD status register.
    STAT = 0xFF41,

    /// Background viewport Y position register.
    SCY = 0xFF42,

    /// Background viewport X position register.
    SCX = 0xFF43,

    /// Window Y position register.
    WY = 0xFF4A,

    /// Window X position register.
    WX = 0xFF4B,

    /// BG palette data register.
    BGP = 0xFF47,

    /// Background color palette index register.
    BCPI = 0xFF68,

    /// Background color palette data register.
    BGPD = 0xFF69,

    /// Object color pallete index register.
    OBPI = 0xFF6A,

    /// Object color palette data register.
    OBPD = 0xFF6B,

    /// VRAM bank register.
    VBK = 0xFF4F,

    /// Prepare speed switch register.
    SPD = 0xFF4D,

    /// VRAM DMA source high (write-only).
    HDMA1 = 0xFF51,

    /// VRAM DMA source low (write-only).
    HDMA2 = 0xFF52,

    /// VRAM DMA destination high (write-only).
    HDMA3 = 0xFF53,

    /// VRAM DMA destination low (write-only).
    HDMA4 = 0xFF54,

    /// VRAM DMA length/mode/start.
    HDMA5 = 0xFF55,

    /// WRAM bank.
    SVBK = 0xFF70,

    /// Interrupt enable register.
    IE = 0xFFFF,
};

/// @brief All possible addresses available to interrupt services.
enum class InterruptVector : uint16_t {
    VBlank = 0x0040,
    Lcd = 0x0048,
    Timer = 0x0050,
    Serial = 0x0058,
    Joypad = 0x0060,
};

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
    uint8_t
    read_byte(const uint16_t address) const;

    [[nodiscard]]
    uint16_t
    read_word(const uint16_t address) const;

    [[nodiscard]]
    uint8_t
    read_io_reg(const IoMap reg) const;

    void
    write_byte(const uint16_t address, const uint8_t value);

    void
    write_word(const uint16_t address, const uint16_t value);

    void
    write_io_reg(const IoMap reg, const uint8_t value);

private:
    std::array<uint8_t, MEMORY_BUS_SIZE> m_bus;
};
} // namespace cocoa::gb

#endif // COOCA_GB_MEMORY_HPP
