#include "calculator_inner_functions_vector_partition_function_1.h"

void Calculator::initializeFunctionsVectorPartitionFunctions() {
  STACK_TRACE("Calculator::initializeFunctionsVectorPartitionFunctions");
  Function::Options innerStandard = Function::Options::standard();
  Function::Options composite = Function::Options::compositeStandard();
  this->addOperationHandler(
    Calculator::Functions::Names::vectorPartitionFunction,
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
    ");VectorPartitionFunction(\n"
    "Sequence{}1,"
    "Sequence{}2,"
    "Sequence{}3,"
    "Sequence{}4)",
    "CalculatorFunctionsVectorPartitionFunction"
    "::vectorPartitionFunctionFormula",
    "VectorPartitionFunction",
    innerStandard
  );
  this->addOperationHandler(
    "ConeDecomposition",
    CalculatorFunctionsVectorPartitionFunction::coneDecompositionSpannedSlices,
    "",
    "Work in progress. \n"
    "Cone decomposition related to a vector partition function. "
  "Uses only slicing plances spanned by n-1 vectors. ",
    "ConeDecomposition(\n"
    "(1,0,0),\n"
    "(0,1,0),\n"
    "(0,0,1),\n"
    "(1,1,0),\n"
    "(0,1,1),\n"
    "(1,1,1)\n"
    ");\n"
    /*    "ConeDecomposition(\n"
    "(1,0,0,0),"
    "(0,1,0,0),"
    "(0,0,1,0),"
    "(0,0,0,1),"
    "(1,1,0,0),"
    "(0,1,1,0),"
    "(0,0,1,1),"
    "(1,1,1,0),"
    "(0,1,1,1),"
    "(1,1,1,1)"
    ");\n"*/
    ,
    "CalculatorFunctionsVectorPartitionFunction::"
  "coneDecompositionSpannedSlices",
    "ConeDecomposition",
    innerStandard
  );
  this->addOperationHandler(
    "ConeDecompositionArbitrarySlices",
    CalculatorFunctionsVectorPartitionFunction::coneDecompositionArbitrarySlices,
    "",
    "Work in progress. \n"
    "Cone decomposition related to a vector partition function. "
  "Unlike ConeDecomposition, this decomposition uses arbitrary combinatorial chamber walls.",
    "ConeDecompositionArbitrarySlices(\n"
    "(1,0,0),\n"
    "(0,1,0),\n"
    "(0,0,1),\n"
    "(1,1,0),\n"
    "(0,1,1),\n"
    "(1,1,1)\n"
    ");\n"
    /*    "ConeDecompositionArbitrarySlices(\n"
    "(1,0,0,0),"
    "(0,1,0,0),"
    "(0,0,1,0),"
    "(0,0,0,1),"
    "(1,1,0,0),"
    "(0,1,1,0),"
    "(0,0,1,1),"
    "(1,1,1,0),"
    "(0,1,1,1),"
    "(1,1,1,1)"
    ");\n"*/
    ,
    "CalculatorFunctionsVectorPartitionFunction::"
  "coneDecompositionSpannedSlices",
    "ConeDecomposition",
    innerStandard
  );
  this->addOperationHandler(
    Calculator::BuiltInTypes::Names::vectorPartitionFunction,
    CalculatorFunctionsVectorPartitionFunction::
    applyVectorPartitionFunctionFormula,
    "",
    "<b>WORK IN PROGRESS</b>. "
    "The value of f{}(4,5) computed below is not accurate. "
    "Will hopefully be fixed in the coming weeks. "
    "Applies the vector partition function formula. ",
    "vectors = (\n"
    "(1,0),"
    "(0,1),"
    "(1,1),"
    "(1,2),"
    "(1,3),"
    "(2,3) "
    ");\n"
    "f=VectorPartitionFunction{} (vectors);\n"
    "target = (4,5);\n"
    "AllVectorPartitions (target, vectors);\n"
    "f{}target;\n"
    "g=VectorPartitionFunction(Sequence{}1,Sequence{}2,Sequence{}3);\n"
    "g{}(10);\n"
    "All"
    "g{}(1,2)",
    "CalculatorFunctionsVectorPartitionFunction"
    "::applyVectorPartitionFunctionFormula",
    "ApplyVectorPartitionFunction",
    composite
  );
}
