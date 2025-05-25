cmake_minimum_required(VERSION 3.29)
project(C_Project_Int3_Cayeux_Mialaret_main C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(C_Project_Int3_Cayeux_Mialaret_main
    bmp24.c
    bmp24.h
    bmp8.c
    bmp8.h
    main.c)
