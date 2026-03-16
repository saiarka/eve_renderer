cmake_minimum_required(VERSION 4.2.3)
project("eve_renderer")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "" FORCE)
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("src")
