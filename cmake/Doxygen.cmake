# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: MIT

option(ENABLE_DOXYGEN "Generate documentation with Doxygen" OFF)

if(ENABLE_DOXYGEN)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    set(doxygen_in ${CMAKE_CURRENT_SOURCE_DIR}/data/doxyfile.in)
    set(doxygen_out ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)
    configure_file(${doxygen_in} ${doxygen_out} @ONLY)

    # INVARIANT: Auto update doxygen build whenever doc comments change.
    message("Doxygen build started")
    add_custom_target(
      doc_doxygen ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${doxygen_out}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM)
  else()
    message(FATAL_ERROR "Doxygen not found")
  endif()
endif()
