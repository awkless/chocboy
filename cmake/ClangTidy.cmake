# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

option(ENABLE_CLANG_TIDY "Use clang-tidy for static analysis" OFF)

if(ENABLE_CLANG_TIDY)
  find_program(clang_tidy_exe NAMES clang-tidy)
  if(clang_tidy_exe)
    set(clang_tidy_checks
      -checks=-*,clang-diagnostic-*,clang-analyzer-*,bugprone-*,modernize-*
      -checks=-modernize-use-trailing-return-type,portability-*,performance-*
      -checks=-modernize-avoid-c-arrays
      -checks=readability-*,-readability-braces-around-statements
      -checks=-readability-magic-numbers)

    if(MSVC)
      set(CMAKE_CXX_CLANG_TIDY
          ${clang_tidy_exe}
          -extra-arg=/EHsc
          ${clang_tidy_checks}
      )
    else()
      set(CMAKE_CXX_CLANG_TIDY
          ${clang_tidy_exe}
          -extra-arg=-Wno-unknown-warning-option
          ${clang_tidy_checks}
      )
    endif()

    if(WARNINGS_AS_ERRORS)
      list(APPEND CMAKE_CXX_CLANG_TIDY -warnings-as-errors=*)
    endif()
  else()
    message(WARNING "clang-tidy not found")
  endif()
endif()

