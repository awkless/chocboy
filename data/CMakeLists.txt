# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

set(config_in_hpp "${CMAKE_CURRENT_SOURCE_DIR}/config.in.hpp")
set(config_out_hpp "${CMAKE_BINARY_DIR}/src/chocboy/config.hpp")
configure_file(${config_in_hpp} ${config_out_hpp} @ONLY)

