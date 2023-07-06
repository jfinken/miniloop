cmake_minimum_required(VERSION 3.10)

project(miniJSON VERSION 1.0.0)

set(miniJSON_BRIEF  "Lightweight cpp wrapper around json-c")

include(${PROJECT_SOURCE_DIR}/cmake/SetEnv.cmake)

add_subdirectory(${LIBRARY_FOLDER})
