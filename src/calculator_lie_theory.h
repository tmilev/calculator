// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef CALCULATOR_LIE_THEORY_INCLUDED
#define CALCULATOR_LIE_THEORY_INCLUDED
#include "calculator.h"

class CalculatorLieTheory {
public:
  static bool killingForm(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerKLcoeffs(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2old(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2inner(Calculator& calculator, BranchingData& theG2B3Data, Expression& output);
  static bool innerLittelmannOperator(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAnimateLittelmannPaths(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLSPath(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTestMonomialBaseConjecture(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerHWVCommon(Calculator& calculator,
    Expression& output,
    Vector<RationalFunction<Rational> >& highestWeightFundCoords,
    Selection& selectionParSel,
    ExpressionContext& hwContext,
    SemisimpleLieAlgebra* owner,
    bool Verbose = true
  );
  static bool innerWriteGenVermaModAsDiffOperatorInner(
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
  static bool innerPrintB3G2branchingTableCharsOnly(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerPrintB3G2branchingIntermediate(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vectors<RationalFunction<Rational> >& theHWs,
    BranchingData& theG2B3Data,
    ExpressionContext &theContext
  );
  static bool innerPrintB3G2branchingTable(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerPrintB3G2branchingTableCommon(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vectors<RationalFunction<Rational> >& outputHWs,
    BranchingData& theG2B3Data,
    ExpressionContext &theContext
  );
  static bool innerWriteGenVermaModAsDiffOperators(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool AllGenerators,
    bool useNilWeight,
    bool ascending
  );
  static bool innerWriteGenVermaModAsDiffOperatorsGeneratorOrder(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorLieTheory::innerWriteGenVermaModAsDiffOperators(calculator, input, output, false, false, true);
  }
  static bool innerWriteGenVermaModAsDiffOperatorsNilOrderDescending(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorLieTheory::innerWriteGenVermaModAsDiffOperators(calculator, input, output, false, true, false);
  }
  static bool innerWriteGenVermaModAsDiffOperatorsAllGensNilOrderDescending(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorLieTheory::innerWriteGenVermaModAsDiffOperators(calculator, input, output, true, false, false);
  }
  static bool innerPrintGenVermaModule(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerWriteGenVermaModAsDiffOperatorUpToLevel(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerHighestWeightVector(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSplitGenericGenVermaTensorFD(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSplitFDpartB3overG2Init(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    BranchingData& theG2B3Data,
    ExpressionContext& outputContext
  );
  static bool innerPrintB3G2branchingTableInit(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    BranchingData& theG2B3data,
    int& desiredHeight,
    ExpressionContext& outputContext
  );
  static bool innerSplitFDpartB3overG2CharsOutput(
    Calculator& calculator, const Expression& input, Expression& output, BranchingData& theG2B3Data
  );
  static bool innerSplitFDpartB3overG2CharsOnly(Calculator& calculator, const Expression& input, Expression& output);
};

#endif
