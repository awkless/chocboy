# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)

if("${srcdir}" STREQUAL "${bindir}")
  message(FATAL_ERROR "In-source builds not allowed, please use build directory")
endif()
