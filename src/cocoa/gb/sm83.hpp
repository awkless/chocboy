// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_GB_SM83_HPP
#define COCOA_GB_SM83_HPP

#include <array>
#include <cstdint>
#include <exception>
#include <memory>
#include <string>

#include <spdlog/logger.h>

#include "cocoa/gb/memory.hpp"
#include "cocoa/utility.hpp"

namespace cocoa::gb {
// NOTE: Excludes 0xCB, because it represents the prefix to an opcode rather than a full
// instruction.
constexpr size_t NO_PREFIX_INSTR_TABLE_SIZE = 255;

constexpr size_t CB_PREFIX_INSTR_TABLE_SIZE = 256;

/// @brief 8-bit register addressing.
enum class Reg8 { B, C, IndirHramC, D, E, H, L, IndirHL, A };

/// @brief 16-bit register addressing.
enum class Reg16 { BC, DE, HL, SP };

/// @brief 16-bit register addressing related to stack operations.
enum class Reg16Stack {
    BC,
    DE,
    HL,
    AF,
};

/// @brief 16-bit register addressing related to 8-bit indirect memory access.
enum class Reg16Indir {
    BC,
    DE,
    HLI,
    HLD,
};

/// @brief 8-bit immediate addressing.
enum class Imm8 {
    Direct,
    IndirHram,
    IndirAbsolute,
};

/// @brief 16-bit immediate addressing.
enum class Imm16 {
    Direct,
    IndirAbsolute,
};

/// @brief CPU flags available.
enum class Flag { Z = 7, N = 6, H = 5, C = 4 };

/// @brief Conditional flag states for control flow instructions.
enum class Condition { NZ, Z, NC, C };

/// @brief Modes of execution for SM83 CPU.
enum class Sm83Mode {
    Running,
    Halted,
    Stopped,
};

/// @brief State of SM83 CPU.
///
/// This contains any state needed for an instruction implementation to function correctly.
struct Sm83State final {
    enum RegIndex {
        A = 0,
        F = 1,
        B = 2,
        C = 3,
        D = 4,
        E = 5,
        H = 6,
        L = 7,
    };

    std::array<uint8_t, 8> regs;
    size_t mcycles;
    size_t tstates;
    MemoryBus& bus;
    Sm83Mode mode;
    uint16_t sp;
    uint16_t pc;
    bool ime;

    explicit Sm83State(MemoryBus& memory);

    /// @brief Load using 8-bit register addressing.
    ///
    /// @return 8-bit value stored directly or indirectly by register.
    template <enum Reg8 R>
    [[nodiscard]]
    constexpr uint8_t
    load_reg8() const;

    /// @brief Store using 8-bit register addressing.
    ///
    /// @param [in] 8-bit value to store directly or indirectly into a register.
    template <enum Reg8 R>
    constexpr void
    store_reg8(const uint8_t value);

    /// @brief Load using 16-bit register addressing.
    ///
    /// @return 16-bit value stored directly from 16-bit register.
    template <enum Reg16 R>
    [[nodiscard]]
    constexpr uint16_t
    load_reg16() const;

    /// @brief Load using 16-bit register addressing for stack manipulation.
    ///
    /// @note Mainly used by instruction implementations that manipulate the SM83 stack.
    ///
    /// @return 16-bit value stored directly in 16-bit register.
    template <enum Reg16Stack R>
    [[nodiscard]]
    constexpr uint16_t
    load_reg16_stack() const;

    /// @brief Load using 16-bit register addressing for indirect 8-bit memory access.
    ///
    /// Issues side effect where HL register will be incremented or decremented by one for each
    /// call to this method when using `Reg16Indir::{HLI, HLD}`.
    ///
    /// @note Mainly used by instructions that allow loading of the A register indirectly through
    ///       16-bit register.
    ///
    /// @return 8-bit value stored at indirect memory address pointed to by 16-bit register.
    template <enum Reg16Indir R>
    [[nodiscard]]
    constexpr uint8_t
    load_reg16_indir();

    /// @brief Store using 16-bit addressing.
    ///
    /// @param [in] 16-bit value to store directly into 16-bit register.
    template <enum Reg16 R>
    constexpr void
    store_reg16(const uint16_t value);

    /// @brief Store using 16-bit addressing for stack manipulation.
    ///
    /// @note Mainly used by instruction implementations that manipulate the SM83 stack.
    ///
    /// @param [in] 16-bit value to store directly into 16-bit register.
    template <enum Reg16Stack R>
    constexpr void
    store_reg16_stack(const uint16_t value);

    /// @brief Store using 16-bit addressing for indirect 8-bit memory access.
    ///
    /// Issues side effect where HL register will be incremented or decremented by one for each
    /// call to this method when using `Reg16Indir::{HLI, HLD}`.
    ///
    /// @note Mainly used by instructions that allow storing of the A register indirectly through
    ///       16-bit register.
    ///
    /// @param [in] 8-bit value to store indirectly at memory address pointed to by 16-bit register.
    template <enum Reg16Indir R>
    constexpr void
    store_reg16_indir(const uint8_t value);

    /// @brief Load 8-bit value through immediate addressing.
    ///
    /// Issues side effect where PC register will be incremented by a certain amount of bytes for a
    /// given mode:
    ///
    /// - `Imm8::Direct` advances PC by 1.
    /// - `Imm8::IndirHram` advances PC by 1.
    /// - `Imm8::IndirAbsolute` advances PC by 2.
    ///
    /// @return Immediate 8-bit value.
    template <enum Imm8 I>
    [[nodiscard]]
    constexpr uint8_t
    load_imm8();

    /// @brief Store 8-bit value through immediate addressing.
    ///
    /// Issues side effect where PC register will be incremented by a certain amount of bytes for a
    /// given mode:
    ///
    /// - `Imm8::IndirHram` advances PC by 1.
    /// - `Imm8::IndirAbsolute` advances PC by 2.
    ///
    /// @note `Imm8::Direct` addressing cannot be used here.
    ///
    /// @param [in] 8-bit value to store.
    template <enum Imm8 I>
    constexpr void
    store_imm8(const uint8_t value);

    /// @brief Load 16-bit value through immediate addressing.
    ///
    /// Issues side effect where PC register will be incremented by a certain amount of bytes for a
    /// given mode:
    ///
    /// - `Imm16::Direct` advances PC by 2.
    ///
    /// @note `Imm16::IndirAbsolute` cannot be used here.
    ///
    /// @return Immediate 16-bit value.
    template <enum Imm16 I>
    constexpr uint16_t
    load_imm16();

    /// @brief Store 16-bit value through immediate addressing.
    ///
    /// Issues side effect where PC register will be incremented by a certain amount of bytes for a
    /// given mode:
    ///
    /// - `Imm16::IndirAbsolute` advances PC by 2.
    ///
    /// @note `Imm16::Direct` cannot be used here.
    ///
    /// @param [in] 16-bit value store.
    template <enum Imm16 I>
    constexpr void
    store_imm16(const uint16_t value);

    /// @brief Set target flag in F register.
    ///
    /// @invariant Only effects high nibble of F register.
    template <enum Flag F>
    constexpr void
    set_flag();

    /// @brief Clear target flag in F register.
    ///
    /// @invariant Only effects high nibble of F register.
    template <enum Flag F>
    constexpr void
    clear_flag();

    /// @brief Toggle target flag based on condition.
    ///
    /// If condition is true, then flag will be set. Otherwise, it will be cleared.
    ///
    /// @invariant Only effects high nibble of F register.
    template <enum Flag F>
    constexpr void
    conditional_flag_toggle(bool condition);

    /// @brief Toggle target flag in F register.
    ///
    /// Functions the same as complementing the flag.
    ///
    /// @invariant Only effects high nibble of F register.
    template <enum Flag F>
    constexpr void
    toggle_flag();

    /// @brief Check if target flag is set in F register.
    ///
    /// @invariant Only effects high nibble of F register.
    template <enum Flag F>
    [[nodiscard]]
    constexpr bool
    is_flag_set() const;

    /// @brief Check if condition is set in F register.
    ///
    /// @invariant Only effects high nibble of F register.
    /// @invariant Only checks Z or C flags based on target condition.
    template <enum Condition C>
    [[nodiscard]]
    constexpr bool
    is_condition_set() const;
};

/// @brief SM83 instruction implementation.
///
/// Used to represent a given instruction after decoding a given opcode.
struct Instruction final {
    std::string_view mnemonic;
    size_t length;
    size_t mcycles;
    size_t tstates;
    void (*execute)(Sm83State&) = nullptr;
};

/// @brief SM83 CPU.
///
/// The Game Boy uses an 8-bit CPU identified as the SM83 CPU core in old Sharp datasheets.
/// It was designed to resemble a "modified" Zilog Z80, but has more in common with the Intel 8080.
/// The SM83 ISA is based on both the Zilog Z80 and Intel 8080. Despite the similarities, the SM83
/// is its own separate architecture with its own unique quirks.
///
/// One of the more unique quirks about the SM83 is that it does not have a dedicated I/O bus, nor
/// dedicated IN/OUT instructions. All I/O is done through memory mapping only. In fact, the SM83
/// introduces HRAM specific instrucitons like `LDH A, n8`, because most I/O registers are mapped
/// inside of HRAM itself.
///
/// @see https://gekkio.fi/files/gb-docs/gbctr.pdf
/// @see https://gbdev.io/pandocs/CPU_Comparison_with_Z80.html
class Sm83 final {
public:
    Sm83(std::shared_ptr<spdlog::logger> log, MemoryBus& memory);

    /// @brief Step through one instruction execution.
    ///
    /// Performs fetch, decode, execute cycle for one instruction based on whatever byte(s) the PC
    /// register is pointing to in memory bus.
    ///
    /// @throws `IllegalOpcode` if any of the 11 illegal opcode instructions are encountered.
    void
    step();

    /// @brief Get current m-cycle count.
    ///
    /// @return m-cycle count.
    [[nodiscard]]
    size_t
    mcycles() const;

    /// @brief Get current t-state count.
    ///
    /// @return t-state count.
    [[nodiscard]]
    size_t
    tstates() const;

private:
    std::array<Instruction, NO_PREFIX_INSTR_TABLE_SIZE> m_no_prefix_instr;
    std::array<Instruction, CB_PREFIX_INSTR_TABLE_SIZE> m_cb_prefix_instr;
    Sm83State m_state;
    std::shared_ptr<spdlog::logger> m_log;
};

class IllegalOpcode final : public std::exception {
public:
    explicit IllegalOpcode(std::string message);

    const char*
    what() const noexcept;

private:
    std::string m_message;
};
} // namespace cocoa::gb

#include "cocoa/gb/sm83.tpp"

#endif // COCOA_GB_SM83_HPP
