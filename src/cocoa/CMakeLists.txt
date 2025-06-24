# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

add_library(cocoa)
target_sources(cocoa
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/utilities.hpp")
target_include_directories(cocoa PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(cocoa
  PRIVATE chocboy::dependencies
          chocboy::cppstd_flags
          chocboy::warning_flags)
add_library(cocoa::cocoa ALIAS cocoa)
