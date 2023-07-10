if( ${BUILD_EXAMPLE} )
  project(${LIBRARY_NAME}_example)

  set(CMAKE_CXX_STANDARD          17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  add_executable(${PROJECT_NAME} main.cpp)

  target_link_libraries(${PROJECT_NAME} ${LIBRARY_NAME})

  install(
    TARGETS                      "${PROJECT_NAME}"
    RUNTIME         DESTINATION  "${CMAKE_INSTALL_BINDIR}"
  )

endif()
