# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

add_library(cocoa)
target_sources(cocoa
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/gb/memory.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/gb/cpu.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/utility.hpp"
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/gb/memory.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/gb/cpu.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/utility.tpp")
target_include_directories(cocoa PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(cocoa
  PRIVATE chocboy::dependencies
          chocboy::cppstd_flags
          chocboy::warning_flags)
add_library(cocoa::cocoa ALIAS cocoa)

if(ENABLE_TESTS)
  find_package(Catch2 REQUIRED)
  include(CTest)
  include(Catch)
  enable_testing()

  add_executable(cocoa_tests)
  target_sources(cocoa_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/utility_test.cpp")
  target_link_libraries(cocoa_tests
    PRIVATE cocoa::cocoa
            chocboy::dependencies
            chocboy::cppstd_flags
            chocboy::warning_flags
            Catch2::Catch2WithMain)
  catch_discover_tests(cocoa_tests)
endif()
