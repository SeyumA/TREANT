add_library(py_decision_tree
  SHARED
    implementation/c_cpp_interface.cpp
    implementation/cpp_implementation.cpp
    ${CMAKE_SOURCE_DIR}/src/SplitOptimizer.cpp
    ${CMAKE_SOURCE_DIR}/src/Dataset.cpp
    ${CMAKE_SOURCE_DIR}/src/DecisionTree.cpp
    ${CMAKE_SOURCE_DIR}/src/Attacker.cpp
    ${CMAKE_SOURCE_DIR}/src/Constraint.cpp
    ${CMAKE_SOURCE_DIR}/src/FeatureColumn.cpp
    ${CMAKE_SOURCE_DIR}/src/Node.cpp
    ${CMAKE_SOURCE_DIR}/src/utils.cpp
  )

# TODO change to relative path ${CMAKE_SOURCE_DIR}
SET(NLOPT_INSTALL_DIR "/home/dg/source/repos/uni/nlopt-2.6.1/install")

find_path(_nlopt_h
  NAMES
    nlopt.hpp
  HINTS
    ${NLOPT_INSTALL_DIR}/include
  )
message(STATUS "NLOPT header file FOUND: ${_nlopt_h}")

target_include_directories(py_decision_tree
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${_nlopt_h}
  )

find_library(_nlopt_lib
  NAMES
    nlopt
  HINTS
    ${NLOPT_INSTALL_DIR}/lib
)
message(STATUS "NLOPT single dynamic library FOUND: ${_nlopt_lib}")

target_link_libraries(py_decision_tree
  PRIVATE
    ${_nlopt_lib}
)

# generate py_decision_tree_export.h
include(GenerateExportHeader)
generate_export_header(py_decision_tree
  BASE_NAME py_decision_tree
  )
