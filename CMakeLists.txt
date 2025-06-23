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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(cmake/InSourceCheck.cmake)
