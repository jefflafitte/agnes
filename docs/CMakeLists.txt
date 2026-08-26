cmake_minimum_required(VERSION 3.24)

project(agnes_docs LANGUAGES NONE)

find_package(Doxygen REQUIRED)

include(FetchContent)

FetchContent_Declare(
  doxygen-awesome-css
  GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
  GIT_TAG        v2.4.2
  SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-awesome-css)

FetchContent_MakeAvailable(doxygen-awesome-css)

file(GLOB_RECURSE AGNES_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h")

set(DOXYFILE       "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile")
set(DOXYGEN_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/html/index.html")

add_custom_command(
  OUTPUT  ${DOXYGEN_OUTPUT}
  DEPENDS ${DOXYFILE} ${AGNES_HEADERS}
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  MAIN_DEPENDENCY ${DOXYFILE}
  COMMENT "Generating documentation"
  VERBATIM)

add_custom_target(docs ALL DEPENDS ${DOXYGEN_OUTPUT})
