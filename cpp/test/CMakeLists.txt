cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 11)

add_executable(dvbs2_test test.cpp)

# копируем DLL рядом с exe
add_custom_command(TARGET dvbs2_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_LIST_DIR}/../build/dvbs2.dll"
        $<TARGET_FILE_DIR:dvbs2_test>
)