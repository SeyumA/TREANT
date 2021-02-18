#include "py_decision_tree.h"
#include "cpp_implementation.hpp"

#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)

py_decision_tree_context_t *py_decision_tree_new() {
  return AS_TYPE(py_decision_tree_context_t, new PyDecisionTree());
}

void py_decision_tree_free(py_decision_tree_context_t *context) { delete AS_TYPE(PyDecisionTree, context); }

void py_decision_tree_predict(const py_decision_tree_context_t *context,
                              const double *X,
                              const unsigned rows,
                              const unsigned cols,
                              double *predictions, const bool score, const bool isRowsWise) {
  return AS_CTYPE(PyDecisionTree, context)->predict(X, rows, cols, predictions, score, isRowsWise);
}

void py_decision_tree_fit(py_decision_tree_context_t *context,
                          const double *X,
                          const unsigned rows,
                          const unsigned cols,
                          const double *y,
                          const char *isNumerical,
                          const char *notNumericalEntries,
                          const char *columnNames,
                          const char *attackerFile,
                          const double budget,
                          const unsigned threads,
                          const bool useICML2019,
                          const unsigned maxDepth,
                          const unsigned int minPerNode,
                          const bool isAffine) {

  return AS_TYPE(PyDecisionTree, context)->fit(X, rows, cols, y, isNumerical, notNumericalEntries, columnNames,
                                               attackerFile,
                                               budget,
                                               threads,
                                               useICML2019,
                                               maxDepth,
                                               minPerNode,
                                               isAffine);
}

bool py_decision_tree_is_trained(const py_decision_tree_context_t *context) {
  return AS_CTYPE(PyDecisionTree, context)->is_trained();
}

void py_decision_tree_load(py_decision_tree_context_t *context, const char *filePath) {
  return AS_TYPE(PyDecisionTree, context)->load(filePath);
}

void py_decision_tree_save(const py_decision_tree_context_t *context, const char *filePath) {
  return AS_CTYPE(PyDecisionTree, context)->save(filePath);
}

void py_decision_tree_pretty_print(const py_decision_tree_context_t *context) {
  return AS_CTYPE(PyDecisionTree, context)->pretty_print();
}
