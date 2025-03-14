#include "calculator_inner_functions_vector_partition_function_1.h"

void Calculator::initializeFunctionsVectorPartitionFunctions() {
  STACK_TRACE("Calculator::initializeFunctionsVectorPartitionFunctions");
  Function::Options innerStandard = Function::Options::standard();
  Function::Options composite = Function::Options::compositeStandard();
  Function::Options administrativeNotTested =
  Function::Options::administrativeNotTested();
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
    "VectorPartitionFunctionWithDetails",
    CalculatorFunctionsVectorPartitionFunction::
    vectorPartitionFunctionFormulaWithDetails,
    "",
    "Computes the vector partition function. "
    "Shows all the internals of the computation, if those are not too long. ",
    "VectorPartitionFunctionWithDetails(\n"
    "(1,0,0),\n"
    "(0,1,0),\n"
    "(0,0,1),\n"
    "(1,1,0),\n"
    "(0,1,1),\n"
    "(1,1,1)\n"
    ");\n",
    "CalculatorFunctionsVectorPartitionFunction"
    "::vectorPartitionFunctionFormula",
    "VectorPartitionFunctionWithDetails",
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
    "Cone decomposition related to a vector partition function. "
    "Uses only slicing plances spanned by n-1 vectors. "
    "If you use a single argument that is a Dynkin type, this function "
    "automatically computes the cone decomposition with respect "
    "to the positve roots of the type. Works up to type B_4. "
    "Larger types will be rejected as their computation run time "
    "is measured in hours or worse.",
    "ConeDecomposition(\n"
    "(1,0,0),\n"
    "(0,1,0),\n"
    "(0,0,1),\n"
    "(1,1,0),\n"
    "(0,1,1),\n"
    "(1,1,1)\n"
    ");\n"
    "ConeDecomposition(G_2);\n",
    "CalculatorFunctionsVectorPartitionFunction::"
    "coneDecompositionSpannedSlicesNoHistory",
    "ConeDecomposition",
    innerStandard
  );
  this->addOperationHandler(
    "ConeDecompositionWithoutAmalgamation",
    CalculatorFunctionsVectorPartitionFunction::
    coneDecompositionSpannedSlicesNoHistoryNoAmalgamation,
    "",
    "A faster version of ConeDecomposition that "
    "produces possibly finer (more) chambers "
    "than are actually needed. "
    "Runs considerably faster.",
    "ConeDecomposition(D_4);",
    "CalculatorFunctionsVectorPartitionFunction::"
    "coneDecompositionSpannedSlicesNoHistoryNoAmalgamation",
    "ConeDecompositionWithoutAmalgamation",
    innerStandard
  );
  this->addOperationHandler(
    "ConeDecompositionArbitrarySlices",
    CalculatorFunctionsVectorPartitionFunction::
    coneDecompositionArbitrarySlices,
    "",
    "Cone decomposition related to a vector partition function. "
    "Unlike ConeDecomposition, this decomposition "
    "uses arbitrary combinatorial chamber walls.",
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
    "ConeDecompositionArbitrarySlicesWithHistory",
    CalculatorFunctionsVectorPartitionFunction::
    coneDecompositionArbitrarySlicesWithHistory,
    "",
    "Same as ConeDecompositionArbitrarySlices but prints out "
    "the intermediate steps. ",
    "ConeDecompositionArbitrarySlicesWithHistory(\n"
    "(1,0,0),\n"
    "(0,1,0),\n"
    "(0,0,1),\n"
    "(1,1,0),\n"
    "(0,1,1),\n"
    "(1,1,1)\n"
    ");\n",
    "CalculatorFunctionsVectorPartitionFunction::"
    "coneDecompositionArbitrarySlicesWithHistory",
    "ConeDecompositionArbitrarySlicesWithHistory",
    innerStandard
  );
  this->addOperationHandler(
    this->builtInName<VectorPartitionFunction>(),
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
  this->addOperationHandler(
    Calculator::Functions::Names::lattice,
    CalculatorFunctionsVectorPartitionFunction::lattice,
    "",
    "Generates a built-in lattice from the given input vectors. ",
    "l = Lattice((1,2,3), (2,2,2), (3,3,9));\n",
    "CalculatorFunctionsVectorPartitionFunction"
    "::lattice",
    Calculator::Functions::Names::lattice,
    innerStandard
  );
  this->addOperationHandler(
    "SubLatticeScalarProductTimesDirectionInLattice",
    CalculatorFunctionsVectorPartitionFunction::
    subLatticeScalarProductTimesDirectionInLattice,
    "",
    "We are given a lattices L and T, "
    "a vector v and d. We are looking for the lattice X "
    "given by: X= {a\\in L: &lt; a, v&gt; d \\in T }. "
    "This function is a useful tool when computing with vector "
    "partition functions. "
    "First argument: L. Second argument: T. Both must be lattices. "
    "Third argument: v. Fourth argument: d. "
    "of vectors w in L for which the scalar product of w and v is integer. ",
    "L = Lattice((2,0), (0,3));\n"
    "T=Lattice((7,0),(0,9));\n"
    "SubLatticeScalarProductTimesDirectionInLattice(L, T, (1,2), (2,3));",
    "CalculatorFunctionsVectorPartitionFunction"
    "::subLatticeScalarProductTimesDirectionInLattice",
    "SubLatticeScalarProductTimesDirectionInLattice",
    innerStandard
  );
  this->addOperationHandler(
    "SubLatticeWithIntegralScalarProducts",
    CalculatorFunctionsVectorPartitionFunction::
    subLatticeWithIntegralScalarProducts,
    "",
    "Given a lattice L and a vector v, generates the sub-lattice W "
    "of vectors w in L for which the scalar product of w and v is integer. ",
    "l = Lattice((1,2,3), (2,2,2), (3,3,9));\n"
    "SubLatticeWithIntegralScalarProducts((1/2,1/3,1/7), l);",
    "CalculatorFunctionsVectorPartitionFunction"
    "::subLatticeWithIntegralScalarProducts",
    "SubLatticeWithIntegralScalarProducts",
    innerStandard
  );
  this->addOperationHandler(
    "DualLattice",
    CalculatorFunctionsVectorPartitionFunction::dualLattice,
    "",
    "Computes the dual lattice of a given lattice. "
    "The dual of the lattice is the lattice of vectors with "
    "integral scalar products with the given vectors. ",
    "l = Lattice((1,2,3), (2,2,2), (3,3,9));\n"
    "DualLattice(l)",
    "CalculatorFunctionsVectorPartitionFunction"
    "::subLatticeWithIntegralScalarProducts",
    "DualLattice",
    innerStandard
  );
  this->addOperationHandler(
    "ReduceModLattice",
    CalculatorFunctionsVectorPartitionFunction::reduceModuloLattice,
    "",
    "Reduces a vector modulo a lattice. "
    "First argument: vector. "
    "Second argument: lattice. ",
    "l = Lattice((1,2,3), (2,2,2), (3,3,9));\n"
    "ReduceModLattice((1,1,1),l);\n"
    "ReduceModLattice((1/2,1/3,1/4),l);\n"
    "ReduceModLattice((1,2),l);\n",
    "CalculatorFunctionsVectorPartitionFunction"
    "::reduceModuloLattice",
    "ReduceModLattice",
    innerStandard
  );
  this->addOperationHandler(
    "QuotientLatticeRepresentatives",
    CalculatorFunctionsVectorPartitionFunction::quotientLatticeRepresentatives,
    "",
    "Computes all representatives of a quotient lattice L/M. "
    "Both arguments must be lattices. ",
    "l = Lattice((1,0,0), (0,1,0), (0,0,1));\n"
    "m = Lattice((1,2,3), (2,2,2), (3,3,9));\n"
    "QuotientLatticeRepresentatives(l,m)",
    "CalculatorFunctionsVectorPartitionFunction"
    "::quotientLatticeRepresentatives",
    "QuotientLatticeRepresentatives",
    innerStandard
  );
  this->addOperationHandler(
    "PlotLatticeRestricted",
    CalculatorFunctionsVectorPartitionFunction::plotLatticeRestricted,
    "",
    "Plots a lattice. "
    "The first argument must be a lattice. "
    "The second optional argument is a list of the normals "
    "of a restricting cone. All points in the lattice whose scalar "
    "product with any of the restricting vectors is zero will be omitted. ",
    "l = Lattice((1,1,0), (0,2,0), (0,0,1));\n"
    "PlotLatticeRestricted(l);\n"
    "m = Lattice((1,1), (0,2));\n"
    "PlotLatticeRestricted(m,((1,0), (0,1)) );\n"
    "PlotLatticeRestricted(m,((-1,1), (1,0)) );\n",
    "CalculatorFunctionsVectorPartitionFunction"
    "::plotLatticeRestricted",
    "PlotLatticeRestricted",
    innerStandard
  );
  this->addOperationHandler(
    "KostantPartitionFunctionsLatex",
    CalculatorFunctionsVectorPartitionFunction::kostantPartitionFunctionLatex,
    "",
    "Requires a logged-in admin to use. "
    "Acts similarly to the "
    "VectorPartitionFunction but 1) displays latex only, "
    "2) acts on Dynkin types only and "
    "3) allows multiple dynkin type inputs "
    "producing a single chunk of latex output. "
    "This function is needed for the internal "
    "documentation of our algorithms, "
    "use VectorPartitionFunction directly instead. ",
    "KostantPartitionFunctionsLatex(\n"
    "a_2,\n"
    "b_2,\n"
    "c_2,\n"
    "g_2,\n"
    "a_3,\n"
    "b_3,\n"
    "c_3,\n"
    "a_4\n"
    ")",
    "CalculatorFunctionsVectorPartitionFunction"
    "::kostantPartitionFunctionLatex",
    "KostantPartitionFunctionsLatex",
    administrativeNotTested
  );
}
