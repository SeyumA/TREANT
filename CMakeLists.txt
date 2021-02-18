cmake_minimum_required(VERSION 3.9)
project (treeant LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#enable_testing()

IF (NOT WIN32)
    IF(NOT CMAKE_BUILD_TYPE)
        SET(CMAKE_BUILD_TYPE Release)
    endif(NOT CMAKE_BUILD_TYPE)
ENDIF ()

SET(CMAKE_CXX_STANDARD 17)

IF (WIN32)
    SET(CMAKE_CXX_FLAGS "/Zi /EHsc /MP /Wall /W4")
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
        include/BaggingClassifier.h

        # Sources
        src/utils.cpp
        src/Logger.cpp
        src/OptParser.cpp
        src/DecisionTree.cpp
        src/Dataset.cpp
        src/Node.cpp
        src/SplitOptimizer.cpp
        src/Attacker.cpp
        src/Constraint.cpp
        src/BaggingClassifier.cpp)


SET(TREANT_LIB treantLib)

add_library(${TREANT_LIB}
        SHARED
        ${SOURCES}
        )

include(FindPkgConfig)
find_package(Nlopt REQUIRED)
include_directories(${NLOPT_INCLUDE_DIR})
target_link_libraries(${TREANT_LIB}
        PRIVATE
        ${NLOPT_LIBRARY}
        )

# Build the executable in the main
add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} ${TREANT_LIB})

# python interface and sources
add_subdirectory(py_decision_tree)

# Add tests
add_test(
  NAME
    python_test
  COMMAND
    ${CMAKE_COMMAND} -E env PY_DECISION_TREE_HEADER_FILE=${CMAKE_CURRENT_SOURCE_DIR}/py_decision_tree/py_decision_tree.h
                            PY_DECISION_TREE_LIBRARY_FILE=${CMAKE_CURRENT_BINARY_DIR}/cmake-build-release/libpy_decision_tree.so
    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_py_decision_tree.py
  )


#add_executable(test_opt_parser tests/test_opt_parser.cpp $<TARGET_OBJECTS:treeant_lib>)
#add_test(NAME test_opt_parser COMMAND test_opt_parser)
#
#add_executable(test_decision_tree tests/test_decision_tree.cpp $<TARGET_OBJECTS:treeant_lib>)
#add_test(NAME test_decision_tree COMMAND test_decision_tree)
#
#add_executable(test_utils_format tests/test_utils_format.cpp $<TARGET_OBJECTS:treeant_lib>)
#add_test(NAME test_utils_format COMMAND test_utils_format)
#
#add_executable(test_utils_format tests/test_parallel.cpp $<TARGET_OBJECTS:treeant_lib>)
#add_test(NAME test_parallel COMMAND test_parallel)
