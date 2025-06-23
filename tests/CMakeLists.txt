# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

add_executable(unit_tests)
target_sources(unit_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/unit_example.cpp")
target_link_libraries(unit_tests
  PRIVATE chocboy::dependencies
          chocboy::cppstd_flags
          chocboy::warning_flags
          Catch2::Catch2WithMain)
catch_discover_tests(unit_tests)
