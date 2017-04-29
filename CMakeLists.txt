cmake_minimum_required(VERSION 3.7)
project(boggle)

set(CMAKE_CXX_STANDARD 11)
include(FindPkgConfig)
pkg_check_modules(CURSES ncurses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})


set(SOURCE_FILES
        board.c
        boggle.h
        check.c
        checkword.c
        ui.c)

add_executable(boggle ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})