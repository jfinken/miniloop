set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES   
    Loop.cpp
#AlarmClock.cpp
    Timeout.cpp
    UnixSignal.cpp
    IO.cpp)

set(HEADERS_PUBLIC 
    Loop.h)

include(${PROJECT_SOURCE_DIR}/cmake/miniLoopConfig.cmake)
