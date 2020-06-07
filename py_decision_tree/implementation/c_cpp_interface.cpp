#include "py_decision_tree.h"
#include "cpp_implementation.hpp"

#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

py_decision_tree_context_t *py_decision_tree_new(unsigned int maxDepth) {
  return AS_TYPE(py_decision_tree_context_t, new PyDecisionTree(maxDepth));
}

void py_decision_tree_free(py_decision_tree_context_t *context) { delete AS_TYPE(PyDecisionTree, context); }

bool py_decision_tree_predict(const py_decision_tree_context_t *context) {
  return AS_CTYPE(PyDecisionTree, context)->predict();
}

void py_decision_tree_fit(py_decision_tree_context_t *context,
                          const char *datasetFile,
                          const char *attackerFile,
                          const double budget,
                          const unsigned threads) {
							  
  return AS_TYPE(PyDecisionTree, context)->fit(datasetFile,
                                               attackerFile,
                                               budget,
                                               threads);
}

bool py_decision_tree_is_trained(const py_decision_tree_context_t *context) {
  return AS_CTYPE(PyDecisionTree, context)->is_trained();
}

void py_decision_tree_pretty_print(const py_decision_tree_context_t *context) {
  return AS_CTYPE(PyDecisionTree, context)->pretty_print();
}
