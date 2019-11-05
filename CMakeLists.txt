cmake_minimum_required(VERSION 3.1)
project (treeant)

enable_testing()

IF (NOT WIN32)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif(NOT CMAKE_BUILD_TYPE)
ENDIF ()

set(CMAKE_CXX_STANDARD 17)

IF (WIN32)
    set(CMAKE_CXX_FLAGS "/Zi /EHsc /MP /Wall /W4")
ELSE ()
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -pthread -fPIC -march=native")
    SET(CMAKE_CXX_FLAGS_RELEASE "-Werror -O3 -s")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
ENDIF ()

# Additional flags if needed
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} <...flags...>")

include_directories(include)

set(SOURCES
        # Headers
        include/utils.h
        include/Logger.h
        include/OptParser.h
        include/DecisionTree.h
        # Sources
        src/utils.cpp
        src/Logger.cpp
        src/OptParser.cpp
        src/DecisionTree.cpp)

# Build an object library
add_library(treeant_lib OBJECT ${SOURCES})

# Build the executable in the main
add_executable(${CMAKE_PROJECT_NAME} main.cpp $<TARGET_OBJECTS:treeant_lib>)

# Add tests
add_executable(test_opt_parser tests/test_opt_parser.cpp $<TARGET_OBJECTS:treeant_lib>)
add_test(NAME test_opt_parser COMMAND test_opt_parser)