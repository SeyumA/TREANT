cmake_minimum_required(VERSION 3.1)
project (treeant)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

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
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -pthread -fPIC -march=native -lm")
    SET(CMAKE_CXX_FLAGS_RELEASE "-Werror -O3 -s")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
ENDIF ()

# Additional flags if needed
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} <...flags...>")

include_directories(include)

set(SOURCES
        # Headers
        include/utils.h
        include/types.h
        include/Logger.h
        include/OptParser.h
        include/Node.h
        include/DecisionTree.h
        include/Dataset.h
        include/SplitOptimizer.h
        include/Attacker.h
        include/Constraint.h

        # Sources
        src/utils.cpp
        src/Logger.cpp
        src/OptParser.cpp
        src/DecisionTree.cpp
        src/Dataset.cpp
        src/Node.cpp
        src/SplitOptimizer.cpp
        src/Attacker.cpp
        src/Constraint.cpp)


# Build the executable in the main
add_executable(${CMAKE_PROJECT_NAME} main.cpp ${SOURCES})

include(FindPkgConfig)
find_package(Nlopt REQUIRED)
include_directories(${NLOPT_INCLUDE_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} ${NLOPT_LIBRARY})


## Add tests
#add_executable(test_opt_parser tests/test_opt_parser.cpp $<TARGET_OBJECTS:treeant_lib>)
#add_test(NAME test_opt_parser COMMAND test_opt_parser)
#
#add_executable(test_decision_tree tests/test_decision_tree.cpp $<TARGET_OBJECTS:treeant_lib>)
#add_test(NAME test_decision_tree COMMAND test_decision_tree)
#
#add_executable(test_utils_format tests/test_utils_format.cpp $<TARGET_OBJECTS:treeant_lib>)
#add_test(NAME test_utils_format COMMAND test_utils_format)
