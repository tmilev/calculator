// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_inner_functions_vector_partition_function_1.h"

void Calculator::initializeFunctionsVectorPartitionFunctions() {
  STACK_TRACE("Calculator::initializeFunctionsVectorPartitionFunctions");
  Function::Options innerStandard = Function::Options::standard();
  this->addOperationHandler(
  Calculator::Atoms::Names::vectorPartitionFunction
    ,
    CalculatorFunctionsVectorPartitionFunction::vectorPartitionFunctionFormula,
    "",
    "Work in progress. Vector partition function formula. ",
    "VectorPartitionFunction(\n"
    "(1,0,0),\n"
    "(0,1,0),\n"
    "(0,0,1),\n"
    "(1,1,0),\n"
    "(0,1,1),\n"
    "(1,1,1)\n"
    ");\n"
    "VectorPartitionFunction(\n"
    "(1,0),"
    "(0,1),"
    "(1,1),"
    "(1,2),"
    "(1,3),"
    "(2,3) "
    ");",
    "CalculatorFunctionsVectorPartitionFunction"
    "::vectorPartitionFunctionFormula",
    "VectorPartitionFunction",
    innerStandard
  );
  this->addOperationHandler(
    "ConeDecomposition",
    CalculatorFunctionsVectorPartitionFunction::coneDecomposition,
    "",
    "Work in progress. \n"
    "Cone decomposition related to a vector partition function. ",
    "ConeDecomposition(\n"
    "(1,0,0),\n"
    "(0,1,0),\n"
    "(0,0,1),\n"
    "(1,1,0),\n"
    "(0,1,1),\n"
    "(1,1,1)\n"
    ");",
    "CalculatorFunctionsVectorPartitionFunction::coneDecomposition",
    "ConeDecomposition",
    innerStandard
  );
}
