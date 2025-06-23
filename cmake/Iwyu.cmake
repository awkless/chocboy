# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

option(ENABLE_IWYU "Use include-what-you-use to check headers" OFF)

if (ENABLE_IWYU)
  find_program(iwyu_exe NAMES include-what-you-use)
  if(iwyu_exe)
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${iwyu_exe} -Xiwyu --no_comments)
  else()
    message(WARNING "include-what-you-use not found")
  endif()
endif()
