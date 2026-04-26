#ifndef header_calculator_lie_theory_ALREADY_INCLUDED
#define header_calculator_lie_theory_ALREADY_INCLUDED

#include "calculator_interface.h"

class CalculatorLieTheory {
private:
  // If multiplyAByPi is true, then elementA will be multiplied by pi.
  static bool exponentOfAdjointOfThroughDiagonalization(
    Calculator& calculator,
    SemisimpleLieAlgebra& owner,
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& elementA,
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& elementB,
    bool multiplyAByPi,
    Expression& output
  );
public:
  // Converts the input to an element of universal enveloping algebra
  // of the given Sesmisimple lie algebra as follows.
  // - g_i, h_i subexpressions will be converted to Chevalley/Cartan
  // generators.
  // - Algebraic numbers will be respected.
  // - The constant \pi will be converted to the anonymous variable x_1 in
  //   the polynomial ring Polynomial<AlgebraicNumber>.
  //   So, 2\pi g_1^2 g_2 will be converted to the
  //   universal enveloping algebra element g_1^2 g_2 with coefficient 2x_1.
  static bool evaluatesToElementUniversalEnvelopingAlgebraPiAlgebraicNumbers(
    Calculator& calculator,
    const Expression& input,
    ElementUniversalEnveloping<Polynomial<AlgebraicNumber> >& output,
    SemisimpleLieAlgebra& owner
  );
  static bool
  convertElementUniversalEnvelopingRationalFractionToAlgebraicCoefficient(
    ElementUniversalEnveloping<RationalFraction<Rational> >& input,
    ElementUniversalEnveloping<AlgebraicNumber>& output,
    SemisimpleLieAlgebra& owner,
    std::stringstream* commentsOnError
  );
  // Returns true if a given element of a lie algebra is
  // - a product of pi and an algebraic element
  // - algebraic element that doesn't contain pi.
  // When the element is a multiple of pi, the element with
  // pi factored out will be written in the outputWhichElement variable,
  // and outputHasPiMultiple will be set to true.
  // In th other case, outputHasPiMultiple will be set to false
  // and the element will be type-converted and in turn
  // written to outputWhichElement.
  static bool convertsToAlgebraicAsPossibleMultipleOfPi(
    const ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >&
    elementPiAlgebraic,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputWhichElement,
    bool& outputHasPiMultiple,
    std::stringstream* commentsOnError
  );
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
  // Computes root subalgebras and sl(2)'s.
  // When computeRealFormSlTwos is set, attempts to compute the real
  // forms (work in progress).
  // When mustRecompute is set, does not use cached results.
  // When restrictTypes is set, attempts to restrict the computation to
  // a single type of sl(2)-triple, extracted from the second argument
  // of the input function.
  static bool rootSubalgebrasAndSlTwos(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool computeRealFormSlTwos,
    bool mustRecompute
  );
  static bool printRootSubalgebrasAndSlTwos(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::rootSubalgebrasAndSlTwos(
      calculator, input, output, false, false
    );
  }
  static bool printRootSubalgebrasAndSlTwosForceRecompute(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::rootSubalgebrasAndSlTwos(
      calculator, input, output, false, true
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
  static bool precomputeSemisimpleLieAlgebraStructureEverything(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::precomputeSemisimpleLieAlgebraStructureWithOptions(
      calculator, input, output, true
    );
  }
  static bool precomputeSemisimpleLieAlgebraStructureAllButSubalgebras(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorLieTheory::precomputeSemisimpleLieAlgebraStructureWithOptions(
      calculator, input, output, false
    );
  }
  static bool precomputeSemisimpleLieAlgebraStructureWithOptions(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool shouldComputeSemisimpleSubalgebras
  );
  static bool embedSemisimpleAlgebraInSemisimpleAlgebra(
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
  static bool drawWeightSupportWithMultiplicities(
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
  static bool elementsInLieAlgebraImpliedLieAlgebra(
    Calculator& calculator,
    const List<Expression>& inputElements,
    SemisimpleLieAlgebra*& outputOwner,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElements
  );
  static bool elementsInUniversalEnvelopingAlgebraImpliedLieAlgebra(
    Calculator& calculator,
    const List<Expression>& inputElements,
    SemisimpleLieAlgebra*& outputOwner,
    List<ElementUniversalEnveloping<Polynomial<AlgebraicNumber> > >&
    outputElementsPiAlgebraic
  );
  static bool elementsInSameLieAlgebra(
    Calculator& calculator,
    const Expression& input,
    SemisimpleLieAlgebra*& outputOwner,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElements
  );
  static bool elementsInSameLieAlgebraPiAlgebraic(
    Calculator& calculator,
    const Expression& input,
    SemisimpleLieAlgebra*& outputOwner,
    List<ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > >&
    outputElements
  );
  static bool elementsInSameUniversalEnvelopingAlgebra(
    Calculator& calculator,
    const Expression& input,
    SemisimpleLieAlgebra*& outputOwner,
    List<ElementUniversalEnveloping<Polynomial<AlgebraicNumber> > >&
    outputElementsPiAlgebraic
  );
  static bool elementsInLieAlgebraExplicitLieAlgebra(
    Calculator& calculator,
    const Expression& inputAlgebra,
    const List<Expression>& inputElements,
    Expression& outputOnError,
    SemisimpleLieAlgebra*& outputOwner,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElements
  );
  static bool elementsInUniversalEnvelopingAlgebraExplicitLieAlgebra(
    Calculator& calculator,
    const Expression& inputAlgebra,
    const List<Expression>& inputElements,
    SemisimpleLieAlgebra*& outputOwner,
    List<ElementUniversalEnveloping<Polynomial<AlgebraicNumber> > >&
    outputElements
  );
  static bool cartanInvolutionInternal(
    Calculator& calculator, const Expression& input, CartanInvolution& output
  );
  static bool cartanInvolution(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool exponentOfAdjointOf(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool adjointCommonEigenspaces(
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
  static bool printSemisimpleSubalgebrasWithCentralizersRecompute(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printSemisimpleSubalgebrasNoCentralizersRecompute(
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
    bool writeToHD,
    FormatExpressions* format
  );
  static bool rootSubsystem(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool allGeneratorsAsMatrixRow(
    Calculator& calculator, SemisimpleLieAlgebra& owner, Expression& output
  );
};

#endif
