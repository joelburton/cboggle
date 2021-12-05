cmake_minimum_required(VERSION 3.7)
project(boggle)

find_library(ncurses CURSES)

add_executable(boggle board.c check.c ui.c dict.c)
target_link_libraries(boggle ${CURSES_LIBRARIES})

add_executable(checker checker.c dict.c)

set(WORDS "${CMAKE_SOURCE_DIR}/words.dat")
add_custom_command(
        TARGET boggle POST_BUILD
        TARGET checker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${WORDS} ${CMAKE_BINARY_DIR}
)