// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef CALCULATOR_LIE_THEORY_INCLUDED
#define CALCULATOR_LIE_THEORY_INCLUDED
#include "calculator.h"

class CalculatorLieTheory {
public:
  static bool killingForm(Calculator& calculator, const Expression& input, Expression& output);
  static bool kazhdanLuzstigCoeffificents(Calculator& calculator, const Expression& input, Expression& output);
  static bool splitFDpartB3overG2old(Calculator& calculator, const Expression& input, Expression& output);
  static bool splitFDpartB3overG2inner(Calculator& calculator, BranchingData& theG2B3Data, Expression& output);
  static bool littelmannOperator(Calculator& calculator, const Expression& input, Expression& output);
  static bool animateLittelmannPaths(Calculator& calculator, const Expression& input, Expression& output);
  static bool LSPath(Calculator& calculator, const Expression& input, Expression& output);
  static bool testMonomialBaseConjecture(Calculator& calculator, const Expression& input, Expression& output);
  static bool highestWeightVectorCommon(Calculator& calculator,
    Expression& output,
    Vector<RationalFunction<Rational> >& highestWeightFundCoords,
    Selection& selectionParSel,
    ExpressionContext& hwContext,
    SemisimpleLieAlgebra* owner,
    bool Verbose = true
  );
  static bool writeGenVermaModAsDiffOperatorInner(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vectors<Polynomial<Rational> >& theHws,
    ExpressionContext& hwContext,
    Selection& selInducing,
    SemisimpleLieAlgebra* owner,
    bool AllGenerators,
    std::string* xLetter,
    std::string* partialLetter,
    std::string* exponentVariableLetter,
    bool useNilWeight,
    bool ascending
  );
  template<class Coefficient>
  static bool typeHighestWeightParabolic(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vector<Coefficient>& outputWeight,
    Selection& outputInducingSel,
    Expression* outputContext = nullptr
  );
  static bool printB3G2branchingTableCharsOnly(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printB3G2branchingIntermediate(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vectors<RationalFunction<Rational> >& theHWs,
    BranchingData& theG2B3Data,
    ExpressionContext &theContext
  );
  static bool printB3G2branchingTable(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printB3G2branchingTableCommon(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vectors<RationalFunction<Rational> >& outputHWs,
    BranchingData& theG2B3Data,
    ExpressionContext &theContext
  );
  static bool writeGenVermaModAsDiffOperators(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool AllGenerators,
    bool useNilWeight,
    bool ascending
  );
  static bool writeGenVermaModAsDiffOperatorsGeneratorOrder(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorLieTheory::writeGenVermaModAsDiffOperators(calculator, input, output, false, false, true);
  }
  static bool writeGenVermaModAsDiffOperatorsNilOrderDescending(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorLieTheory::writeGenVermaModAsDiffOperators(calculator, input, output, false, true, false);
  }
  static bool writeGenVermaModAsDiffOperatorsAllGensNilOrderDescending(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorLieTheory::writeGenVermaModAsDiffOperators(calculator, input, output, true, false, false);
  }
  static bool printGeneralizedVermaModule(Calculator& calculator, const Expression& input, Expression& output);
  static bool writeGeneralizedVermaModuleAsDifferentialOperatorUpToLevel(Calculator& calculator, const Expression& input, Expression& output);
  static bool highestWeightVector(Calculator& calculator, const Expression& input, Expression& output);
  static bool splitGenericGeneralizedVermaTensorFiniteDimensional(Calculator& calculator, const Expression& input, Expression& output);
  static bool splitFDpartB3overG2(Calculator& calculator, const Expression& input, Expression& output);
  static bool splitFDpartB3overG2Init(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    BranchingData& theG2B3Data,
    ExpressionContext& outputContext
  );
  static bool printB3G2branchingTableInit(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    BranchingData& theG2B3data,
    int& desiredHeight,
    ExpressionContext& outputContext
  );
  static bool splitFDpartB3overG2CharsOutput(
    Calculator& calculator, const Expression& input, Expression& output, BranchingData& theG2B3Data
  );
  static bool splitFDpartB3overG2CharsOnly(Calculator& calculator, const Expression& input, Expression& output);
  static bool rootSAsAndSltwos(
    Calculator& calculator, const Expression& input, Expression& output, bool showSLtwos, bool MustRecompute
  );
  static bool printRootSAs(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorLieTheory::rootSAsAndSltwos(calculator, input, output, false, false);
  }
  static bool printRootSAsForceRecompute(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorLieTheory::rootSAsAndSltwos(calculator, input, output, false, true);
  }
  static bool printSltwos(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorLieTheory::rootSAsAndSltwos(calculator, input, output, true, false);
  }
  static bool weylDimFormula(Calculator& calculator, const Expression& input, Expression& output);
  static bool decomposeCharGenVerma(Calculator& calculator, const Expression& input, Expression& output);
  static bool decomposeFDPartGeneralizedVermaModuleOverLeviPart(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool parabolicWeylGroups(Calculator& calculator, const Expression& input, Expression& output);
  static bool parabolicWeylGroupsBruhatGraph(Calculator& calculator, const Expression& input, Expression& output);
  static bool growDynkinType(Calculator& calculator, const Expression& input, Expression& output);
  static bool getPrincipalSl2Index(Calculator& calculator, const Expression& input, Expression& output);
  static bool getDynkinIndicesSlTwoSubalgebras(Calculator& calculator, const Expression& input, Expression& output);

  static bool constructCartanSubalgebra(Calculator& calculator, const Expression& input, Expression& output);

  static bool getCentralizerChainsSemisimpleSubalgebras(Calculator& calculator, const Expression& input, Expression& output);
  static bool computeSemisimpleSubalgebras(Calculator& calculator, const Expression& input, Expression& output);
  static bool computePairingTablesAndFKFTsubalgebras(Calculator& calculator, const Expression& input, Expression& output);
};

#endif
