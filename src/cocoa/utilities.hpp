// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOA_UTILITIES_HPP
#define COCOA_UTILITIES_HPP

#include <cstdint>
#include <cstddef>
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
} // namespace cocoa

#endif // COCOA_UTILITIES_HPP
