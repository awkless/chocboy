# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

add_library(cppstd_flags INTERFACE)
target_compile_features(cppstd_flags INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
add_library(chocboy::cppstd_flags ALIAS cppstd_flags)

# References for these warnings I found here:
# https://learn.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level?view=msvc-170
set(windows_warning_flags
    /W4
    /w14242
    /w14254
    /w14263
    /w14265
    /w14287
    /we4289
    /w14296
    /w14311
    /w14545
    /w14546
    /w14547
    /w14549
    /w14555
    /w14619
    /w14640
    /w14826
    /w14905
    /w14906
    /w14928
    /permissive-
)

# Clang matches the general settings of GCC, which is very nice. See:
# https://clang.llvm.org/docs/UsersManual.html#command-line-options
set(clang_warning_flags
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2)

if(WARNINGS_AS_ERRORS)
  set(clang_warning_flags ${clang_warning_flags} -Werror)
  set(windows_warning_flags ${windows_warning_flags} /WX)
endif()

# Some extra stuff that GCC offers to make life easier...
set(gcc_warning_flags
    ${clang_warning_flags}
    -Wmisleading-indentation
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wuseless-cast)

if(MSVC)
  set(project_warnings ${windows_warning_flags})
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  set(project_warnings ${clang_warning_flags})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(project_warnings ${gcc_warning_flags})
else()
  set(project_warnings)
  message(
    AUTHOR_WARNING "No warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
endif()

add_library(warning_flags INTERFACE)
target_compile_options(warning_flags INTERFACE ${project_warnings})
add_library(chocboy::warning_flags ALIAS warning_flags)
