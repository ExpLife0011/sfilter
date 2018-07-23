cmake_minimum_required(VERSION 3.10)
project(sfilter)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(example
        example.c
        kdq.h
        khash.h
        kvec.h
        sfilter.c
        sfilter.h)
