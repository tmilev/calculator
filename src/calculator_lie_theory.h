
#ifndef header_calculator_lie_theory_ALREADY_INCLUDED
#define header_calculator_lie_theory_ALREADY_INCLUDED

#include "calculator.h"

class CalculatorLieTheory {
public:
  static bool killingForm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool kazhdanLuzstigCoeffificents(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool splitFDpartB3overG2old(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool splitFDpartB3overG2inner(
    Calculator& calculator, BranchingData& g2B3Data, Expression& output
  );
  static bool littelmannOperator(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool animateLittelmannPaths(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool LSPath(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool testMonomialBaseConjecture(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool highestWeightVectorCommon(
    Calculator& calculator,
    Expression& output,
    Vector<RationalFraction<Rational> >& highestWeightFundamentalCoordinates,
    Selection& selectionParSel,
    ExpressionContext& hwContext,
    SemisimpleLieAlgebra* owner,
    bool Verbose = true
  );
  static bool writeGenVermaModAsDiffOperatorInner(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vectors<Polynomial<Rational> >& highestWeights,
    ExpressionContext& hwContext,
    Selection& selInducing,
    SemisimpleLieAlgebra* owner,
    bool allGenerators,
    std::string* xLetter,
    std::string* partialLetter,
    std::string* exponentVariableLetter,
    bool useNilWeight,
    bool ascending
  );
  template <class Coefficient>
  static bool typeHighestWeightParabolic(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vector<Coefficient>& outputWeight,
    Selection& outputInducingSelection,
    Expression* outputContext = nullptr
  );
  static bool printB3G2branchingTableCharsOnly(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printB3G2branchingIntermediate(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vectors<RationalFraction<Rational> >& highestWeights,
    BranchingData& g2inB3Data,
    ExpressionContext& context
  );
  static bool printB3G2branchingTable(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printB3G2branchingTableCommon(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Vectors<RationalFraction<Rational> >& outputHighestWeights,
    BranchingData& g2InB3Data,
    ExpressionContext& context
  );
  static bool writeGenVermaModAsDiffOperators(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool allGenerators,
    bool useNilWeight,
    bool ascending
  );
  static bool writeGenVermaModAsDiffOperatorsGeneratorOrder(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::writeGenVermaModAsDiffOperators(
      calculator, input, output, false, false, true
    );
  }
  static bool writeGenVermaModAsDiffOperatorsNilOrderDescending(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::writeGenVermaModAsDiffOperators(
      calculator, input, output, false, true, false
    );
  }
  static bool writeGenVermaModAsDiffOperatorsAllGensNilOrderDescending(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::writeGenVermaModAsDiffOperators(
      calculator, input, output, true, false, false
    );
  }
  static bool printGeneralizedVermaModule(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool writeGeneralizedVermaModuleAsDifferentialOperatorUpToLevel(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool highestWeightVector(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool splitGenericGeneralizedVermaTensorFiniteDimensional(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool splitFDpartB3overG2(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool splitFDpartB3overG2Init(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    BranchingData& g2b3Data,
    ExpressionContext& outputContext
  );
  static bool printB3G2branchingTableInit(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    BranchingData& g2b3Data,
    int& desiredHeight,
    ExpressionContext& outputContext
  );
  static bool splitFDpartB3overG2CharsOutput(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    BranchingData& g2B3Data
  );
  static bool splitFDpartB3overG2CharsOnly(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool rootSubalgebrasAndSlTwos(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool showSLTwos,
    bool computeRealFormSlTwos,
    bool mustRecompute
  );
  static bool printRootSubalgebras(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::rootSubalgebrasAndSlTwos(
      calculator, input, output, false, false, false
    );
  }
  static bool printRootSubalgebrasForceRecompute(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::rootSubalgebrasAndSlTwos(
      calculator, input, output, false, false, true
    );
  }
  static bool printSlTwos(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::rootSubalgebrasAndSlTwos(
      calculator, input, output, true, false, false
    );
  }
  static bool weylDimensionFormula(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool decomposeCharGenVerma(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool decomposeFDPartGeneralizedVermaModuleOverLeviPart(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool parabolicWeylGroups(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool parabolicWeylGroupsBruhatGraph(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool growDynkinType(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getPrincipalSl2Index(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getDynkinIndicesSlTwoSubalgebras(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool constructCartanSubalgebra(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getCentralizerChainsSemisimpleSubalgebras(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool computeSemisimpleSubalgebras(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool computePairingTablesAndFKFTsubalgebras(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool casimirWithRespectToLevi(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool generateVectorSpaceClosedWithRespectToLieBracket(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool chooseCartanSubalgebra(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool canBeExtendedParabolicallyTo(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getSymmetricCartan(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool drawRootSystem(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool drawWeightSupportWithMults(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool drawWeightSupport(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool embedG2InB3(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool casimir(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getLinksToSimpleLieAlgebras(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool slTwoRealFormStructure(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool forceRecompute
  );
  static bool slTwoRealFormStructureForceRecompute(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool elementsInSameLieAlgebra(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    SemisimpleLieAlgebra*& outputOwner,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElements
  );
  static bool cartanInvolutionInternal(
    Calculator& calculator,
    const Expression& input,
    CartanInvolution& output
  );
  static bool cartanInvolution(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool adjointCommonEigenSpaces(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool adjointMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool standardRepresentationMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isReductiveLieSubalgebra(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool slTwoRealFormStructureComputeOnDemand(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool characterSemisimpleLieAlgebraFiniteDimensional(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printSemisimpleSubalgebras(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool doForceRecompute,
    bool doAttemptToSolveSystems,
    bool docomputePairingTable,
    bool docomputeModuleDecompositionsition,
    bool doComputeNilradicals,
    bool doAdjustCentralizers
  );
  static bool printSemisimpleSubalgebrasNilradicals(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printSemisimpleSubalgebrasRecompute(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printSemisimpleSubalgebrasNoCentralizers(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printSemisimpleSubalgebrasNoSolutions(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printSemisimpleSubalgebrasRegular(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool chevalleyGenerator(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool cartanGenerator(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printSemisimpleLieAlgebraShort(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::printSemisimpleLieAlgebra(
      calculator, input, output, false
    );
  }
  static bool printSemisimpleLieAlgebraVerbose(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::printSemisimpleLieAlgebra(
      calculator, input, output, true
    );
  }
  static bool writeSemisimpleLieAlgebraToHardDisk(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printSemisimpleLieAlgebra(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool verbose
  );
  static bool writeToHardDiskOrPrintSemisimpleLieAlgebra(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool verbose,
    bool writeToHD
  );
  static bool functionWriteToHardDiskOrPrintSemisimpleLieAlgebra(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool verbose,
    bool writeToHD
  );
  static bool rootSubsystem(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

#endif
