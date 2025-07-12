// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_UTILITY_TPP
#define COCOA_UTILITY_TPP

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace cocoa {
template <typename T, size_t Position>
constexpr void
set_bit(T& var)
{
    static_assert(Position < std::numeric_limits<T>::digits, "position exceeds maximum bit range");
    static_assert(std::is_integral<T>::value == true, "type is not an integral type");
    static_assert(std::is_unsigned<T>::value == true, "type is not an unsigned type");
    var |= static_cast<T>(T(1) << Position);
}

template <typename T, size_t Position>
constexpr void
clear_bit(T& var)
{
    static_assert(Position < std::numeric_limits<T>::digits, "position exceeds maximum bit range");
    static_assert(std::is_integral<T>::value == true, "type is not an integral type");
    static_assert(std::is_unsigned<T>::value == true, "type is not an unsigned type");
    var &= static_cast<T>(~(T(1) << Position));
}

template <typename T, size_t Position>
constexpr bool
is_bit_set(T var)
{
    static_assert(Position < std::numeric_limits<T>::digits, "position exceeds maximum bit range");
    static_assert(std::is_integral<T>::value == true, "type is not an integral type");
    static_assert(std::is_unsigned<T>::value == true, "type is not an unsigned type");
    return (var >> Position) & T(1);
}

template <typename T, size_t Position>
constexpr void
toggle_bit(T& var)
{
    static_assert(Position < std::numeric_limits<T>::digits, "position exceeds maximum bit range");
    static_assert(std::is_integral<T>::value == true, "type is not an integral type");
    static_assert(std::is_unsigned<T>::value == true, "type is not an unsigned type");
    var ^= static_cast<T>((T(1) << Position));
}

template <typename T, size_t Position>
constexpr void
conditional_bit_toggle(T& var, bool condition)
{
    static_assert(Position < std::numeric_limits<T>::digits, "position exceeds maximum bit range");
    static_assert(std::is_integral<T>::value == true, "type is not an integral type");
    static_assert(std::is_unsigned<T>::value == true, "type is not an unsigned type");
    if (condition == true)
        set_bit<T, Position>(var);
    else
        clear_bit<T, Position>(var);
}

template <typename T>
constexpr typename std::underlying_type<T>::type
from_enum(T value)
{
    return static_cast<typename std::underlying_type<T>::type>(value);
}

template <typename T, typename V>
constexpr T
from_pair(V high, V low)
{
    static_assert(std::numeric_limits<V>::digits * 2 == std::numeric_limits<T>::digits,
        "total bits of pair exceeds bit range of target return type");
    constexpr unsigned int shift = std::numeric_limits<T>::digits / 2;
    return static_cast<T>((high << shift) | low);
}

template <typename T, typename V>
constexpr T
from_high(V value)
{
    constexpr unsigned int shift = std::numeric_limits<V>::digits / 2;
    return static_cast<T>(value >> shift);
}

template <typename T, typename V>
constexpr T
from_low(V value)
{
    constexpr unsigned int shift = std::numeric_limits<T>::digits;
    constexpr V mask = (V(1) << shift) - V(1);
    return static_cast<T>(value & mask);
}
} // namespace cocoa

#endif // COCOA_UTILITY_TPP
