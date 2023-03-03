cmake_minimum_required(VERSION 3.22.3)
project(GARDEN_PARADISE C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -ansi -std=c11")

add_executable(
        GARDEN_PARADISE
        main.c
        buildinfo.h
        config.c config.h
        game_control.c game_control.h
        global_enums.h
        global_structs.h
        inhabitant.c inhabitant.h
        simulation.c simulation.h
        utils.c utils.h
)

target_link_libraries(GARDEN_PARADISE)
