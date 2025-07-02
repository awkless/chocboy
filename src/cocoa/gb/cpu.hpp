// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_GB_CPU_HPP
#define COCOA_GB_CPU_HPP

#include <cstdint>
#include <exception>
#include <string>
#include <memory>

#include <spdlog/logger.h>

#include "cocoa/gb/memory.hpp"
#include "cocoa/utilities.hpp"

namespace cocoa::gb {
enum class RegIndex {
    A = 0,
    F = 1,
    B = 2,
    C = 3,
    D = 4,
    E = 5,
    H = 6,
    L = 7,
};

enum class Reg8 {
    B = 0,
    C = 1,
    D = 2,
    E = 3,
    H = 4,
    L = 5,
    HL = 6,
    A = 7,
};

enum class Reg16 {
    BC = 0,
    DE = 1,
    HL = 2,
    SP = 3,
};

enum class Reg16Stk {
    BC = 0,
    DE = 1,
    HL = 2,
    AF = 3,
};

enum class Reg16Mem {
    BC = 0,
    DE = 1,
    HLI = 2,
    HLD = 3,
};

enum class Flag {
    Z = 7,
    N = 6,
    H = 5,
    C = 4,
};

struct Sm83State final {
    size_t cycles;
    uint16_t sp;
    uint16_t pc;
    std::array<uint8_t, 8> regs;
    MemoryBus& bus;

    explicit Sm83State(MemoryBus& memory);

    template <enum Reg16 REG16>
    constexpr uint16_t get_reg16() const
    {
        if constexpr (REG16 == Reg16::BC) {
            return cocoa::from_pair<uint16_t, uint8_t>(
                regs[cocoa::from_enum(RegIndex::B)], regs[cocoa::from_enum(RegIndex::C)]);
        }

        if constexpr (REG16 == Reg16::DE) {
            return cocoa::from_pair<uint16_t, uint8_t>(
                regs[cocoa::from_enum(RegIndex::D)], regs[cocoa::from_enum(RegIndex::E)]);
        }

        if constexpr (REG16 == Reg16::HL) {
            return cocoa::from_pair<uint16_t, uint8_t>(
                regs[cocoa::from_enum(RegIndex::H)], regs[cocoa::from_enum(RegIndex::L)]);
        }

        if constexpr (REG16 == Reg16::SP)
            return sp;
    }

    template <enum Reg16 REG16>
    constexpr void set_reg16(uint16_t value)
    {
        if constexpr (REG16 == Reg16::BC) {
            regs[cocoa::from_enum(RegIndex::B)] = cocoa::from_high(value);
            regs[cocoa::from_enum(RegIndex::C)] = cocoa::from_low(value);
        }

        if constexpr (REG16 == Reg16::DE) {
            regs[cocoa::from_enum(RegIndex::D)] = cocoa::from_high(value);
            regs[cocoa::from_enum(RegIndex::E)] = cocoa::from_low(value);
        }

        if constexpr (REG16 == Reg16::HL) {
            regs[cocoa::from_enum(RegIndex::H)] = cocoa::from_high(value);
            regs[cocoa::from_enum(RegIndex::L)] = cocoa::from_low(value);
        }

        if constexpr (REG16 == Reg16::SP) {
            sp = value;
        }
    }

    template <enum Reg16Stk REG16_STK>
    constexpr uint16_t get_reg16_stk() const
    {
        if constexpr (REG16_STK == Reg16Stk::BC)
            return get_reg16<Reg16::BC>();

        if constexpr (REG16_STK == Reg16Stk::DE)
            return get_reg16<Reg16::DE>();

        if constexpr (REG16_STK == Reg16Stk::HL)
            return get_reg16<Reg16::HL>();

        if constexpr (REG16_STK == Reg16Stk::AF) {
            return cocoa::from_pair<uint16_t, uint8_t>(
                regs[cocoa::from_enum(RegIndex::A)], regs[cocoa::from_enum(RegIndex::F)]);
        }
    }

    template <enum Reg16Stk REG16_STK>
    constexpr void set_reg16_stk(uint16_t value)
    {
        if constexpr (REG16_STK == Reg16Stk::BC)
            set_reg16<Reg16::BC>(value);

        if constexpr (REG16_STK == Reg16Stk::DE)
            set_reg16<Reg16::DE>(value);

        if constexpr (REG16_STK == Reg16Stk::HL)
            set_reg16<Reg16::HL>(value);

        if constexpr (REG16_STK == Reg16Stk::AF) {
            regs[cocoa::from_enum(RegIndex::A)] = cocoa::from_high(value);
            regs[cocoa::from_enum(RegIndex::F)] = cocoa::from_low(value);
        }
    }

    template <enum Reg16Mem REG16_MEM>
    constexpr uint8_t get_reg16_mem()
    {
        if constexpr (REG16_MEM == Reg16Mem::BC) {
            return bus.read_u8(get_reg16<Reg16::BC>());
        }

        if constexpr (REG16_MEM == Reg16Mem::DE) {
            return bus.read_u8(get_reg16<Reg16::DE>());
        }

        if constexpr (REG16_MEM == Reg16Mem::HLI) {
            uint16_t hli = get_reg16<Reg16::HL>();
            uint8_t value = bus.read_u8(hli++);
            set_reg16<Reg16::HL>(hli);
            return value;
        }

        if constexpr (REG16_MEM == Reg16Mem::HLD) {
            uint16_t hli = get_reg16<Reg16::HL>();
            uint8_t value = bus.read_u8(hli--);
            set_reg16<Reg16::HL>(hli);
            return value;
        }
    }

    template <enum Reg16Mem REG16_MEM>
    constexpr void set_reg16_mem(uint8_t value)
    {
        if constexpr (REG16_MEM == Reg16Mem::BC) {
            bus.write_u8(get_reg16<Reg16::BC>(), value);
        }

        if constexpr (REG16_MEM == Reg16Mem::DE) {
            bus.write_u8(get_reg16<Reg16::DE>(), value);
        }

        if constexpr (REG16_MEM == Reg16Mem::HLI) {
            uint16_t hli = get_reg16<Reg16::HL>();
            bus.write_u8(hli++, value);
            set_reg16<Reg16::HL>(hli);
        }

        if constexpr (REG16_MEM == Reg16Mem::HLD) {
            uint16_t hli = get_reg16<Reg16::HL>();
            bus.write_u8(hli--, value);
            set_reg16<Reg16::HL>(hli);
        }
    }

    template <enum Reg8 REG8>
    constexpr uint8_t get_reg8() const
    {
        if constexpr (REG8 == Reg8::B)
            return regs[cocoa::from_enum(RegIndex::B)];
        if constexpr (REG8 == Reg8::C)
            return regs[cocoa::from_enum(RegIndex::C)];
        if constexpr (REG8 == Reg8::D)
            return regs[cocoa::from_enum(RegIndex::D)];
        if constexpr (REG8 == Reg8::E)
            return regs[cocoa::from_enum(RegIndex::E)];
        if constexpr (REG8 == Reg8::H)
            return regs[cocoa::from_enum(RegIndex::H)];
        if constexpr (REG8 == Reg8::L)
            return regs[cocoa::from_enum(RegIndex::L)];
        if constexpr (REG8 == Reg8::HL)
            return bus.read_u8(get_reg16<Reg16::HL>());
        if constexpr (REG8 == Reg8::A)
            return regs[cocoa::from_enum(RegIndex::A)];
    }

    template <enum Reg8 REG8>
    constexpr void set_reg8(uint8_t value)
    {
        if constexpr (REG8 == Reg8::B)
             regs[cocoa::from_enum(RegIndex::B)] = value;
        if constexpr (REG8 == Reg8::C)
             regs[cocoa::from_enum(RegIndex::C)] = value;
        if constexpr (REG8 == Reg8::D)
             regs[cocoa::from_enum(RegIndex::D)] = value;
        if constexpr (REG8 == Reg8::E)
             regs[cocoa::from_enum(RegIndex::E)] = value;
        if constexpr (REG8 == Reg8::H)
             regs[cocoa::from_enum(RegIndex::H)] = value;
        if constexpr (REG8 == Reg8::L)
             regs[cocoa::from_enum(RegIndex::L)] = value;
        if constexpr (REG8 == Reg8::HL)
             bus.write_u8(get_reg16<Reg16::HL>(), value);
        if constexpr (REG8 == Reg8::A)
             regs[cocoa::from_enum(RegIndex::A)] = value;
    }

    template <enum Flag FLAG>
    constexpr void set_flag()
    {
        uint8_t flag = regs[cocoa::from_enum(RegIndex::F)];
        set_bit<uint8_t, cocoa::from_enum(FLAG)>(flag);
        regs[cocoa::from_enum(RegIndex::F)] = flag;
    }

    template <enum Flag FLAG>
    constexpr void clear_flag()
    {
        uint8_t flag = regs[cocoa::from_enum(RegIndex::F)];
        clear_bit<uint8_t, cocoa::from_enum(FLAG)>(flag);
        regs[cocoa::from_enum(RegIndex::F)] = flag;
    }

    template <enum Flag FLAG>
    constexpr void conditional_flag_toggle(bool condition)
    {
        uint8_t flag = regs[cocoa::from_enum(RegIndex::F)];
        conditional_bit_toggle<uint8_t, cocoa::from_enum(FLAG)>(flag, condition);
        regs[cocoa::from_enum(RegIndex::F)] = flag;
    }
};

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
    explicit Sm83(std::shared_ptr<spdlog::logger> log, MemoryBus& bus);

    ~Sm83() noexcept = default;

    void step();

private:
    std::shared_ptr<spdlog::logger> m_log;
    Sm83State m_state;
};

class IllegalOpcode final : public std::exception {
public:
    explicit IllegalOpcode(std::string message);

    const char* what() const noexcept;

private:
    std::string m_message;
};

} // namespace cocoa::gb

#endif // COCOA_GB_CPU_HPP
