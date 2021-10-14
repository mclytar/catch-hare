cmake_minimum_required(VERSION 3.1)

project(CatchHare VERSION 0.1)

configure_file(config.h.in config.h)

include_directories(./io ./game ./test)

add_executable(CatchHare main.c io/io.c game/board.c game/player.c test/test.c)

target_include_directories(CatchHare PUBLIC "${PROJECT_BINARY_DIR}")