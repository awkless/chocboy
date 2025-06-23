// SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
// SPDX-License-Identifier: MIT

#ifndef COCOBOY_CONFIG_HPP
#define COCOBOY_CONFIG_HPP

#include <string_view>

namespace cocoboy {
// Name of program in full.
inline constexpr std::string_view PROGRAM_NAME = "@PROJECT_NAME@";

// Full program version.
inline constexpr std::string_view PROGRAM_VERSION = "@PROJECT_VERSION@";

// Full program description.
inline constexpr std::string_view PROGRAM_DESCRIPTION =
    "A chocolatey sweet GameBoy emulator";

// Program major version number.
inline constexpr int PROGRAM_VERSION_MAJOR = @PROJECT_VERSION_MAJOR@;

// Program minor version number.
inline constexpr int PROGRAM_VERSION_MINOR = @PROJECT_VERSION_MINOR@;

// Program patch version number.
inline constexpr int PROGRAM_VERSION_PATCH = @PROJECT_VERSION_PATCH@;
}  // namespace cc8emu

#endif  // COOCBOY_CONFIG_HPP
