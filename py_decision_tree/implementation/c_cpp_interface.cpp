#include "py_decision_tree.h"
#include "cpp_implementation.hpp"

#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

py_decision_tree_context_t *py_decision_tree_new(const char *datasetFile) {
  return AS_TYPE(py_decision_tree_context_t, new PyDecisionTree(datasetFile));
}

void py_decision_tree_free(py_decision_tree_context_t *context) { delete AS_TYPE(PyDecisionTree, context); }

bool py_decision_tree_predict(const py_decision_tree_context_t *context) {
  return AS_CTYPE(PyDecisionTree, context)->predict();
}

void py_decision_tree_fit(py_decision_tree_context_t *context) {
  return AS_TYPE(PyDecisionTree, context)->fit();
}
