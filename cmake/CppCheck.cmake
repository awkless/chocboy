# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

option(ENABLE_CPPHECK "Use cppcheck for static analysis" OFF)

if(ENABLE_CPPCHECK)
    find_program(cppcheck_exe NAMES cppcheck)
    if(cppcheck_exe)
    set(CMAKE_CXX_CPPCHECK
        ${cppcheck_exe} --enable=style,performance,warning,portability
        --suppress=internalAstError --inline-suppr
        --suppress=unmatchedSuppression --inconclusive)

    if(WARNINGS_AS_ERRORS)
      list(APPEND CMAKE_CXX_CPPCHECK --error-exitcode=2)
    endif()
  else()
    message(WARNING "cppcheck not found")
  endif()
endif()
