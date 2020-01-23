cmake_minimum_required(VERSION 2.8.11)
project(perp-4116-cmd)

add_executable(perp-4116-cmd
        perp-4116-cmd.c
        vgpi-api.c
)

INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/perp-4116-cmd DESTINATION bin)
