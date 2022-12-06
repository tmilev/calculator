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
    ");\n"
    "VectorPartitionFunction(\n"
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
    Calculator::Functions::Names::vectorPartitionFunctionElementary,
    CalculatorFunctionsVectorPartitionFunction::
    vectorPartitionFunctionFormulaElementary,
    "",
    "Work in progress. Vector partition function formula, elementary method. ",
    "VectorPartitionFunctionElementary(\n"
    "(1,1),\n"
    "(1,0),\n"
    "(0,1)\n"
    ");",
    "CalculatorFunctionsVectorPartitionFunction"
    "::vectorPartitionFunctionFormulaElementary",
    "VectorPartitionFunctionElementary",
    innerStandard
  );
  this->addOperationHandler(
    "ConeDecompositionWithHistory",
    CalculatorFunctionsVectorPartitionFunction::
    coneDecompositionSpannedSlicesWithHistory,
    "",
    "Same as ConeDecomposition but shows the history "
    "of which slices were made to get the decomposition.",
    "ConeDecompositionWithHistory(\n"
    "(1,0,0),\n"
    "(0,1,0),\n"
    "(0,0,1),\n"
    "(1,1,0),\n"
    "(0,1,1),\n"
    "(1,1,1)\n"
    ");\n",
    "CalculatorFunctionsVectorPartitionFunction::"
    "coneDecompositionSpannedSlicesWithHistory",
    "ConeDecompositionWithHistory",
    innerStandard
  );
  this->addOperationHandler(
    "ConeDecomposition",
    CalculatorFunctionsVectorPartitionFunction::
    coneDecompositionSpannedSlicesNoHistory,
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
    ");\n",
    "CalculatorFunctionsVectorPartitionFunction::"
    "coneDecompositionSpannedSlicesNoHistory",
    "ConeDecomposition",
    innerStandard
  );
  this->addOperationHandler(
    "ConeDecompositionArbitrarySlices",
    CalculatorFunctionsVectorPartitionFunction::
    coneDecompositionArbitrarySlices,
    "",
    "Work in progress. \n"
    "Cone decomposition related to a vector partition function. "
    "Unlike ConeDecomposition, this decomposition uses arbitrary combinatorial chamber walls."
    ,
    "ConeDecompositionArbitrarySlices(\n"
    "(1,0,0),\n"
    "(0,1,0),\n"
    "(0,0,1),\n"
    "(1,1,0),\n"
    "(0,1,1),\n"
    "(1,1,1)\n"
    ");\n",
    "CalculatorFunctionsVectorPartitionFunction::"
    "coneDecompositionArbitrarySlices",
    "ConeDecompositionArbitrarySlices",
    innerStandard
  );
  this->addOperationHandler(
    Calculator::BuiltInTypes::Names::vectorPartitionFunction,
    CalculatorFunctionsVectorPartitionFunction::
    applyVectorPartitionFunctionFormula,
    "",
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
    "AllVectorPartitions(Sequence{}10, (Sequence{}1,Sequence{}2,Sequence{}3))",
    "CalculatorFunctionsVectorPartitionFunction"
    "::applyVectorPartitionFunctionFormula",
    "ApplyVectorPartitionFunction",
    composite
  );
  this->addOperationHandler(
    "BernoulliSum",
    CalculatorFunctionsVectorPartitionFunction::bernoulliSum,
    "",
    "Returns the Bernoulli sum formula / Faulhaber's formula, "
    "for the sum of the powers of 1^k+2^k+...+n^k as a polynomial of n of k+1'st degree. "
    ,
    "p=3;\n"
    "N=100;\n"
    "B=BernoulliSum(p,n);\n"
    "f{}0= 0;\n"
    "f{}{{n}} = n^p+f{}(n-1);\n"
    "f{}N;\n"
    "(n=N;B)_2;\n",
    "CalculatorFunctionsVectorPartitionFunction"
    "::bernouliSum",
    "BernoulliSum",
    innerStandard
  );
}
