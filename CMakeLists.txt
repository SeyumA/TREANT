cmake_minimum_required(VERSION 3.1)
project (treeant)

IF (NOT WIN32)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif(NOT CMAKE_BUILD_TYPE)
ENDIF ()

IF (WIN32)
    set(CMAKE_CXX_FLAGS "/Zi /std:c++17 /EHsc /MP /Wall /W4")
ELSE ()
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -pthread -std=c++17 -fPIC -march=native")
    SET(CMAKE_CXX_FLAGS_RELEASE "-Werror -O3 -s")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
ENDIF ()

# Additional flags if needed
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} <...flags...>")

include_directories(./include)

set(SOURCES
        # Headers
        include/Logger.h
        include/OptParser.h
        # Sources
        src/Logger.cpp
        src/OptParser.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} main.cpp)
