# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
  "chocboy"
  VERSION 0.1.0
  DESCRIPTION "A chocolatey sweet GameBoy emulator"
  LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(WARNINGS_AS_ERRORS "Treat most build warnings generated as errors" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(cmake/InSourceCheck.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/ClangTidy.cmake)
include(cmake/CppCheck.cmake)
include(cmake/Doxygen.cmake)
include(cmake/Iwyu.cmake)

find_package(SDL3 REQUIRED)
find_package(cxxopts REQUIRED)
find_package(fmt REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)

# INVARIANT: Manually build SDL3 backends for Dear Imgui as static library.
add_library(backends STATIC)
target_sources(
  backends
  PUBLIC "${CMAKE_BINARY_DIR}/src/imgui/backends/imgui_impl_sdl3.h"
         "${CMAKE_BINARY_DIR}/src/imgui/backends/imgui_impl_sdlrenderer3.h"
  PRIVATE "${CMAKE_BINARY_DIR}/src/imgui/backends/imgui_impl_sdl3.cpp"
          "${CMAKE_BINARY_DIR}/src/imgui/backends/imgui_impl_sdlrenderer3.cpp")

target_link_libraries(
  backends
  PRIVATE chocboy::cppstd_flags
          SDL3::SDL3
          imgui::imgui)
add_library(imgui::backends ALIAS backends)
target_include_directories(backends PUBLIC "${CMAKE_BINARY_DIR}/src")

add_library(dependencies INTERFACE)
target_link_libraries(
  dependencies
  INTERFACE spdlog::spdlog
            SDL3::SDL3
            cxxopts::cxxopts
            fmt::fmt
            imgui::imgui
            imgui::backends)
add_library(chocboy::dependencies ALIAS dependencies)

add_subdirectory(data)
add_subdirectory(src)

if(ENABLE_TESTS)
  find_package(Catch2 REQUIRED)
  include(CTest)
  include(Catch)
  enable_testing()
  add_subdirectory(tests)
endif()
