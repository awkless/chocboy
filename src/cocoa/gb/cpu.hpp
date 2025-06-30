// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_GB_CPU_HPP
#define COCOA_GB_CPU_HPP

#include <cstdint>

#include "cocoa/gb/memory.hpp"
#include "cocoa/utilities.hpp"

namespace cocoa::gb {
/// @brief SM83 CPU implementation.
///
/// The GameBoy uses an 8-bit CPU architecture often referred to as the _SM83_. This CPU is a unique
/// piece of hardware that shares many features from Intel 8080 and Zilog Z80, but is a completely
/// separate architecture designed by SHARP for Nintendo.
///
/// The SM83 uses a CISC variable-length instruction set architecture. The first byte of an
/// instruction is considered the _opcode_, i.e., the special bit pattern that determines the type
/// of instruction to execute. Typically, the SM83 may take up to two bytes to identify an opcode
/// due to the use of a _prefix_ byte often referred to as the _CB prefix_ byte, which causes the
/// next opcode byte to be interpreted as bit manipulation instruction.
///
/// This type implements the SM83 instruction set with accurate mcycle timing. In many ways, this
/// type acts as the brain of emulator. It ensures that stuff gets executed through its instruction
/// set. All other peripherals on the GameBoy SoC communicate with this CPU implementation through
/// the memory bus to get stuff done.
///
/// @see https://gbdev.io/pandocs/CPU_Instruction_Set.html
class Sm83 final {
public:
    explicit Sm83(MemoryBus& bus);

    ~Sm83() noexcept = default;

private:
    std::array<uint8_t, 8> m_regs;
    uint16_t m_sp;
    uint16_t m_pc;
    size_t m_cycles;
    MemoryBus& m_bus;

    // clang-format off
    enum class RegIndex { A = 0, F = 1, B = 2, C = 3, D = 4, E = 5, H = 6, L = 7 };
    enum class Reg8 { B = 0, C = 1, D = 2, E = 3, H = 4, L = 5, HL = 6, A = 7 };
    enum class Reg16 { BC = 0, DE = 1, HL = 2, SP = 3 };
    enum class Reg16Stk { BC = 0, DE = 1, HL = 2, AF = 3 };
    enum class Reg16Mem { BC = 0, DE = 1, HLI = 2, HLD = 3 };
    enum class Flag { Z = 7, N = 6, H = 5, C = 4 };
    // clang-format on

    template <enum Reg16 REG16>
    constexpr auto reg16() const
    {
        if constexpr (REG16 == Reg16::BC) {
            return cocoa::from_pair<uint16_t, uint8_t>(
                m_regs[cocoa::from_enum(RegIndex::B)], m_regs[cocoa::from_enum(RegIndex::C)]);
        }

        if constexpr (REG16 == Reg16::DE) {
            return cocoa::from_pair<uint16_t, uint8_t>(
                m_regs[cocoa::from_enum(RegIndex::D)], m_regs[cocoa::from_enum(RegIndex::E)]);
        }

        if constexpr (REG16 == Reg16::HL) {
            return cocoa::from_pair<uint16_t, uint8_t>(
                m_regs[cocoa::from_enum(RegIndex::H)], m_regs[cocoa::from_enum(RegIndex::L)]);
        }

        if constexpr (REG16 == Reg16::SP)
            return m_sp;
    }

    template <enum Reg16Stk REG16_STK>
    constexpr auto reg16_stk() const
    {
        if constexpr (REG16_STK == Reg16Stk::BC)
            return reg16<Reg16::BC>();

        if constexpr (REG16_STK == Reg16Stk::DE)
            return reg16<Reg16::DE>();

        if constexpr (REG16_STK == Reg16Stk::HL)
            return reg16<Reg16::HL>();

        if constexpr (REG16_STK == Reg16Stk::AF) {
            return cocoa::from_pair<uint16_t, uint8_t>(
                m_regs[cocoa::from_enum(RegIndex::A)], m_regs[cocoa::from_enum(RegIndex::F)]);
        }
    }

    template <enum Reg8 REG8>
    constexpr auto reg8() const
    {
        if constexpr (REG8 == Reg8::B)
            return m_regs[cocoa::from_enum(RegIndex::B)];
        if constexpr (REG8 == Reg8::C)
            return m_regs[cocoa::from_enum(RegIndex::C)];
        if constexpr (REG8 == Reg8::D)
            return m_regs[cocoa::from_enum(RegIndex::D)];
        if constexpr (REG8 == Reg8::E)
            return m_regs[cocoa::from_enum(RegIndex::E)];
        if constexpr (REG8 == Reg8::H)
            return m_regs[cocoa::from_enum(RegIndex::H)];
        if constexpr (REG8 == Reg8::L)
            return m_regs[cocoa::from_enum(RegIndex::L)];
        if constexpr (REG8 == Reg8::HL)
            return m_bus.read_u8(reg16<Reg16::HL>());
        if constexpr (REG8 == Reg8::A)
            return m_regs[cocoa::from_enum(RegIndex::A)];
    }
};
} // namespace cocoa::gb

#endif // COCOA_GB_CPU_HPP
