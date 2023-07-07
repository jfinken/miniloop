cmake_minimum_required(VERSION 3.10)

project(miniLoop VERSION 1.0.0)

set(miniLoop_BRIEF  "Lightweight cpp wrapper around libevent")

include(${PROJECT_SOURCE_DIR}/cmake/SetEnv.cmake)

add_subdirectory(${LIBRARY_FOLDER})
