project(miniJSON)

cmake_minimum_required(VERSION 3.10)

set(PROJECT_BRIEF "Lightweight cpp wrapper around json-c")

set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME} )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions("-Wall")

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSON_C REQUIRED json-c)

add_library(
  ${PROJECT_NAME}
  STATIC
  Object.cpp
)

set(
  MINIJSON_PUBLIC_HEADERS
  JSON.h
)

# Directories are created with 0755 permissions
set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
    OWNER_READ
    OWNER_WRITE
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${JSON_C_LIBRARIES})
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${JSON_C_INCLUDE_DIRS}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_DIR}/include>
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE      CXX                         )
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER        "${MINIJSON_PUBLIC_HEADERS}")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME          "${PROJECT_NAME}"           )
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX               ".a"                        )
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX               ""                          )

install( TARGETS "${PROJECT_NAME}"
   ARCHIVE DESTINATION "${INSTALL_DIR}/lib"
   COMPONENT library
   PUBLIC_HEADER DESTINATION "${INSTALL_DIR}/include"
   INCLUDES DESTINATION "${INSTALL_DIR}/include"
)
