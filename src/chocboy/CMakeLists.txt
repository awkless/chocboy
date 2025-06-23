# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

add_executable(chocboy)
target_sources(chocboy  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
target_link_libraries(chocboy
  PRIVATE chocboy::dependencies
          chocboy::cppstd_flags
          chocboy::warning_flags)
set_target_properties(chocboy
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  OUTPUT_NAME chocboy)
