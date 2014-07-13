add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/markdown_parser.c
                   COMMAND leg ${CMAKE_CURRENT_SOURCE_DIR}/markdown_parser.leg -o ${CMAKE_CURRENT_BINARY_DIR}/markdown_parser.c
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/markdown_parser.leg
                   VERBATIM)
add_library(markdownparse
            markdownparse.c markdown_parser.c utility.c parse.c str.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/include)

install(TARGETS markdownparse
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(FILES ${CMAKE_SOURCE_DIR}/include/markdownparse.h
        DESTINATION include)
