find_package(PkgConfig)
pkg_check_modules(GLIB2 REQUIRED glib-2.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GLIB2_INCLUDE_DIRS})

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/markdown_parser.c
                   COMMAND leg ${CMAKE_CURRENT_SOURCE_DIR}/markdown_parser.leg -o ${CMAKE_CURRENT_BINARY_DIR}/markdown_parser.c
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/markdown_parser.leg
                   VERBATIM)
add_library(markdownparse
            markdown_parser.c markdown_output.c markdown_lib.c utility_functions.c
            parsing_functions.c odf.c)
