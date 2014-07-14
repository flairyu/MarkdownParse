include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(test_build EXCLUDE_FROM_ALL test.c)
set_target_properties(test_build PROPERTIES OUTPUT_NAME test)
add_dependencies(test_build markdownparse)
target_link_libraries(test_build markdownparse)

add_custom_target(test
                  ${CMAKE_CURRENT_SOURCE_DIR}/test.py
                  VERBATIM)
add_dependencies(test test_build)
