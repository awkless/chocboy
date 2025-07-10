// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_GB_CPU_HPP
#define COCOA_GB_CPU_HPP

#include <array>
#include <cstdint>
#include <exception>
#include <string>
#include <memory>

#include <spdlog/logger.h>

#include "cocoa/gb/memory.hpp"
#include "cocoa/utility.hpp"

namespace cocoa::gb {
enum class Reg8 { A, F, B, C, D, E, H, L };
enum class Reg16 { AF, BC, DE, HL, SP };
enum class Flag { Z = 7, N = 6, H = 5, C = 4 };
enum class Condition { NZ, Z, NC, C };

struct RegisterFile final {
    // TODO: Remove yucky magic numbers for some method that initializes register state based on
    // GameBoy hardware model being used.
    std::array<uint8_t, 8> regs = { 0x01, 0x80, 0x00, 0x13, 0x00, 0xD8, 0x01, 0x4D };
    uint16_t sp = 0xFFFE;
    uint16_t pc = 0x0100;

    template <enum Reg8 R>
    [[nodiscard]]
    constexpr uint8_t load_reg8() const;

    template <enum Reg8 R>
    [[nodiscard]]
    constexpr uint8_t load_reg8_hram_indirect(const MemoryBus& bus) const;

    template <enum Reg8 R>
    constexpr void store_reg8(const uint8_t value);

    template <enum Reg8 R>
    constexpr void store_reg8_hram_indirect(MemoryBus& bus, const uint8_t value);

    template <enum Reg16 R>
    [[nodiscard]]
    constexpr uint16_t load_reg16() const;

    template <enum Reg16 R>
    [[nodiscard]]
    constexpr uint8_t load_reg16_indirect(const MemoryBus& bus) const;

    template <enum Reg16 R>
    [[nodiscard]]
    constexpr uint8_t load_reg16_inc_indirect(const MemoryBus& bus);

    template <enum Reg16 R>
    [[nodiscard]]
    constexpr uint8_t load_reg16_dec_indirect(const MemoryBus& bus);

    template <enum Reg16 R>
    constexpr void store_reg16(const uint16_t value);

    template <enum Reg16 R>
    constexpr void store_reg16_indirect(MemoryBus& bus, const uint8_t value);

    template <enum Reg16 R>
    constexpr void store_reg16_inc_indirect(MemoryBus& bus, const uint8_t value);

    template <enum Reg16 R>
    constexpr void store_reg16_dec_indirect(MemoryBus& bus, const uint8_t value);

    template <enum Flag F>
    constexpr void set_flag();

    template <enum Flag F>
    constexpr void clear_flag();

    template <enum Flag F>
    constexpr void conditional_flag_toggle(bool condition);

    template <enum Flag F>
    constexpr void toggle_flag();

    template <enum Flag F>
    [[nodiscard]]
    constexpr bool is_flag_set() const;

    template <enum Condition C>
    [[nodiscard]]
    constexpr bool is_condition_set() const;
};

enum class ExecutionMode {
    Running,
    Halted,
    Stopped,
};

struct Sm83State final {
    size_t mcycles;
    RegisterFile reg;
    MemoryBus& bus;
    ExecutionMode mode;

    explicit Sm83State(MemoryBus& memory);
};

struct Instruction final {
    std::string_view mnemonic;
    size_t mcycles;
    size_t length;
    void (*execute)(Sm83State&) = nullptr;
};

class Sm83 final {
public:
    Sm83(std::shared_ptr<spdlog::logger> log, MemoryBus& memory);

    void step();

    [[nodiscard]]
    size_t mcycles() const;

private:
    std::array<Instruction, 256> m_no_prefix_instr;
    std::array<Instruction, 256> m_cb_prefix_instr;
    Sm83State m_state;
    std::shared_ptr<spdlog::logger> m_log;
};

class IllegalOpcode final : public std::exception {
public:
    explicit IllegalOpcode(std::string message);

    const char* what() const noexcept;

private:
    std::string m_message;
};
} // namespace cocoa::gb

#include "cocoa/gb/sm83.tpp"

#endif // COCOA_GB_CPU_HPP
