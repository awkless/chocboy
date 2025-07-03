// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_UTILITIES_HPP
#define COCOA_UTILITIES_HPP

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace cocoa {
/// @brief Set target bit in variable.
///
/// @pre Given variable must be an unsigned integral type.
/// @pre Position must range between [0, n), where _n_ is the total number of bits available in
///      target variable.
///
/// @param [in,out] var Target variable to set bit inside of.
template <typename T, size_t Position>
constexpr void set_bit(T& var)
{
    static_assert(Position < std::numeric_limits<T>::digits, "position exceeds maximum bit range");
    static_assert(std::is_integral<T>::value == true, "type is not an integral type");
    static_assert(std::is_unsigned<T>::value == true, "type is not an unsigned type");
    var |= static_cast<T>(T(1) << Position);
}

/// @brief Clear target bit in variable.
///
/// @pre Given variable must be an unsigned integral type.
/// @pre Position must range between [0, n), where _n_ is the total number of bits available in
///      target variable.
///
/// @param [in,out] var Target variable to clear bit inside of.
template <typename T, size_t Position>
constexpr void clear_bit(T& var)
{
    static_assert(Position < std::numeric_limits<T>::digits, "position exceeds maximum bit range");
    static_assert(std::is_integral<T>::value == true, "type is not an integral type");
    static_assert(std::is_unsigned<T>::value == true, "type is not an unsigned type");
    var &= static_cast<T>(~(T(1) << Position));
}

/// @brief Test if target bit is set.
///
/// @pre Given variable must be an unsigned integral type.
/// @pre Position must range between [0, n), where _n_ is the total number of bits available in
///      target variable.
///
/// @param [in] var Target variable to test bit inside of.
/// @return True if bit is set, false otherwise.
template <typename T, size_t Position>
constexpr bool is_bit_set(T var)
{
    static_assert(Position < std::numeric_limits<T>::digits, "position exceeds maximum bit range");
    static_assert(std::is_integral<T>::value == true, "type is not an integral type");
    static_assert(std::is_unsigned<T>::value == true, "type is not an unsigned type");
    return (var >> Position) & T(1);
}

/// @brief Toggle target bit in variable.
///
/// @pre Given variable must be an unsigned integral type.
/// @pre Position must range between [0, n), where _n_ is the total number of bits available in
///      target variable.
///
/// @param [in,out] var Target variable to toggle bit inside of.
template <typename T, size_t Position>
constexpr void toggle_bit(T& var)
{
    static_assert(Position < std::numeric_limits<T>::digits, "position exceeds maximum bit range");
    static_assert(std::is_integral<T>::value == true, "type is not an integral type");
    static_assert(std::is_unsigned<T>::value == true, "type is not an unsigned type");
    var ^= static_cast<T>((T(1) << Position));
}

/// @brief Toggle target bit based on condition.
///
/// If condition is true, then bit will be set. Otherwise, bit will be cleared.
///
/// @pre Given variable must be an unsigned integral type.
/// @pre Position must range between [0, n), where _n_ is the total number of bits available in
///      target variable.
///
/// @param [in,out] var Target variable to conditionally toggle bit inside of.
template <typename T, size_t Position>
constexpr void conditional_bit_toggle(T& var, bool condition)
{
    static_assert(Position < std::numeric_limits<T>::digits, "position exceeds maximum bit range");
    static_assert(std::is_integral<T>::value == true, "type is not an integral type");
    static_assert(std::is_unsigned<T>::value == true, "type is not an unsigned type");
    if (condition == true)
        set_bit<T, Position>(var);
    else
        clear_bit<T, Position>(var);
}

/// @brief Convert enum into integral type.
///
/// @param [in] value Member of enum to convert.
/// @return Member of enum converted to target type.
template <typename T = int>
constexpr typename std::underlying_type<T>::type from_enum(T value)
{
    return static_cast<typename std::underlying_type<T>::type>(value);
}

/// @brief Obtain full type from a pair of smaller types.
///
/// @pre The sum of variable pair bits must equal the total number of bits of target type.
///
/// @param [in] high Variable of pair acting as high bits.
/// @param [in] low Variable of pair acting as low bits.
/// @return New type composed of variable pair merged together.
template <typename T = uint16_t, typename V = uint8_t>
constexpr T from_pair(V high, V low)
{
    static_assert(std::numeric_limits<V>::digits * 2 == std::numeric_limits<T>::digits,
        "total bits of pair exceeds bit range of target return type");
    constexpr unsigned int shift = std::numeric_limits<T>::digits / 2;
    return static_cast<T>((high << shift) | low);
}

/// @brief Get high bits of value.
///
/// @param [in] value Value to extract high bits from.
/// @return High bits of value.
template <typename T = uint8_t, typename V = uint16_t>
constexpr T from_high(V value)
{
    constexpr unsigned int shift = std::numeric_limits<V>::digits / 2;
    return static_cast<T>(value >> shift);
}

/// @brief Get low bits of value.
///
/// @param [in] value Value to extract low bits from.
/// @return Low bits of value.
template <typename T = uint8_t, typename V = uint16_t>
constexpr T from_low(V value)
{
    constexpr unsigned int shift = std::numeric_limits<T>::digits;
    constexpr V mask = (V(1) << shift) - V(1);
    return static_cast<T>(value & mask);
}
} // namespace cocoa

#endif // COCOA_UTILITIES_HPP
