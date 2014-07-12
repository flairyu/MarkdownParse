find_package(PkgConfig)
pkg_check_modules(GLIB2 REQUIRED glib-2.0)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/markdown_parser.c
                   COMMAND leg ${CMAKE_CURRENT_SOURCE_DIR}/markdown_parser.leg -o ${CMAKE_CURRENT_BINARY_DIR}/markdown_parser.c
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/markdown_parser.leg
                   VERBATIM)
add_library(markdownparse
            markdownparse.c markdown_parser.c utility.c parse.c odf.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GLIB2_INCLUDE_DIRS}
                    ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(markdownparse ${GLIB2_LIBRARIES})
