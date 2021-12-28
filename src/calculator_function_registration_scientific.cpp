// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_inner_functions_vector_partition_function_1.h"

void Calculator::initializeFunctionsVectorPartitionFunctions() {
  MacroRegisterFunctionWithName("Calculator::initializeFunctionsScientificBasic");
  Function::Options innerStandard = Function::Options::standard();

  this->addOperationHandler(
    "VectorPartitionFunction",
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
    ");",
    "CalculatorFunctionsVectorPartitionFunction::vectorPartitionFunctionFormula",
    "VectorPartitionFunction",
    innerStandard
  );
}

