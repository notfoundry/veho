
set(source_files
        ${CMAKE_CURRENT_SOURCE_DIR}/test_bus_builder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sam3x/rev_8e.hpp
)
add_executable(veho_test ${source_files})
target_link_libraries(veho_test INTERFACE veho)
target_include_directories(veho_test PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/third_party
        )

set_property(TARGET veho_test PROPERTY CXX_STANDARD 11)

enable_testing()
add_test(NAME veho_tests COMMAND veho_test)