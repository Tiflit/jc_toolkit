cmake_minimum_required(VERSION 3.15)
project(jc_toolkit VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libjctool)
add_subdirectory(cli)
