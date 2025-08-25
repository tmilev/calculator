#ifndef header_math_extra_lie_theory_extras_ALREADY_INCLUDED
#define header_math_extra_lie_theory_extras_ALREADY_INCLUDED

#include "html_routines.h"
#include "math_extra_semisimple_lie_algebras.h"
#include "math_extra_universal_enveloping.h"
#include "math_rational_function.h"
#include "math_vector_partition_functions.h"

// The following data is isolated in a struct because it is
// way too large a lump to pass separately.
struct BranchingData {
  HomomorphismSemisimpleLieAlgebra homomorphism;
  FormatExpressions format;
  Vector<RationalFraction<Rational> > weightFundamentalCoordinates;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> >
  ambientCharacter;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> >
  smallCharacterFiniteDimensionalPart;
  Selection inducing;
  Selection smallParabolicSelection;
  Selection splittingParabolicSelection;
  Vectors<Rational> weightsNilradicalLarge;
  Vectors<Rational> weightsNilradicalSmall;
  Vectors<Rational> weightsNilModPreNil;
  List<int> indicesNilradicalSmall;
  List<int> indicesNilradicalLarge;
  List<ElementSemisimpleLieAlgebra<Rational> > nilradicalLarge;
  List<ElementSemisimpleLieAlgebra<Rational> > nilradicalSmall;
  List<ElementSemisimpleLieAlgebra<Rational> >
  nilradicalModuloPreimageNilradical;
  Vectors<RationalFraction<Rational> > outputWeightsFundamentalCoordinates;
  Vectors<RationalFraction<Rational> > outputWeightsSimpleCoordinates;
  Vectors<RationalFraction<Rational> > g2Weights;
  Vectors<RationalFraction<Rational> > g2DualWeights;
  Vectors<RationalFraction<Rational> > leviEigenSpace;
  Vectors<Rational> generatorsSmallSub;
  HashedList<RationalFraction<Rational> > characterDifferences;
  List<ElementUniversalEnveloping<RationalFraction<Rational> > >
  outputEigenWords;
  List<RationalFraction<Rational> > allCharacters;
  List<ElementSumGeneralizedVermas<RationalFraction<Rational> > > eigenVectors;
  List<ElementUniversalEnveloping<RationalFraction<Rational> > >
  elementsUniversalEnveloping;
  List<Rational> additionalMultipliers;
  List<RationalFraction<Rational> > shapovalovProducts;
  List<ElementSumGeneralizedVermas<RationalFraction<Rational> > >
  eigenVectorsLevi;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
  weylGroupFiniteDimensional;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
  weylGroupFiniteDimensionalSmallAsSubgroupInLarge;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
  weylGroupFiniteDimensionalSmall;
  bool flagUseNilWeightGeneratorOrder;
  bool flagAscendingGeneratorOrder;
  std::string getStringCasimirProjector(
    int index, const Rational& additionalMultiple
  );
  template <class Coefficient>
  Vector<Coefficient> projectWeight(Vector<Coefficient>& input);
  void resetOutputData();
  void initializePart1NoSubgroups();
  void initializePart2NoSubgroups();
  void initializeAfterParabolicSelectionAndHomomorphism() {
    STACK_TRACE(
      "BranchingData::initializeAfterParabolicSelectionAndHomomorphism"
    );
    this->initializePart1NoSubgroups();
    this->initializePart2NoSubgroups();
  }
  BranchingData();
};

class SemisimpleLieAlgebraOrdered {
public:
  SemisimpleLieAlgebra* ownerSemisimpleLieAlgebra;
  // the format of the order is arbitrary except for the following
  // requirements:
  // -All elements of the order must be either 1) nilpotent or 2) elements of
  // the Cartan
  // -Let the number of positive roots be N and the rank be K. Then the indices
  // N,..., N+K- 1 must
  //  correspond to the elements of the Cartan.
  List<ElementSemisimpleLieAlgebra<Rational> > elementOrder;
  // The order of chevalley generators is as follows. First come negative
  // roots,
  // then elements of cartan, then positive Vectors<Rational>
  // The weights are in increasing order
  // The i^th column of the matrix gives the coordinates of the i^th Chevalley
  // generator
  // in the current coordinates
  Matrix<Rational> chevalleyGeneratorsInCurrentCoordinates;
  void assignGeneratorCoefficientOne(
    int index, ElementSemisimpleLieAlgebra<Rational>& output
  ) {
    output.operator=(this->elementOrder[index]);
  }
  SemisimpleLieAlgebraOrdered();
  bool checkInitialization() const;
  int getDisplayIndexFromGeneratorIndex(int generatorIndex);
  void getLinearCombinationFrom(
    ElementSemisimpleLieAlgebra<Rational>& input,
    Vector<Rational>& coefficients
  );
  void initialize(
    List<ElementSemisimpleLieAlgebra<Rational> >& inputOrder,
    SemisimpleLieAlgebra& owner
  );
  void initDefaultOrder(SemisimpleLieAlgebra& owner);
};

template <class Coefficient>
Vector<Coefficient> BranchingData::projectWeight(Vector<Coefficient>& input) {
  Vector<Coefficient> result;
  Vector<Coefficient> fundamentalCoordinatesSmaller;
  fundamentalCoordinatesSmaller.makeZero(
    this->homomorphism.domainAlgebra().getRank()
  );
  for (int j = 0; j < this->homomorphism.domainAlgebra().getRank(); j ++) {
    fundamentalCoordinatesSmaller[j] =
    this->homomorphism.coDomainAlgebra().weylGroup.rootScalarCartanRoot(
      input, homomorphism.imagesCartanDomain[j]
    );
    fundamentalCoordinatesSmaller[j] /=
    this->homomorphism.domainAlgebra().weylGroup.cartanSymmetric.elements[j][
      j
    ] /
    2;
  }
  result =
  this->homomorphism.domainAlgebra().weylGroup.
  getSimpleCoordinatesFromFundamental(
    fundamentalCoordinatesSmaller, Coefficient::zero()
  );
  return result;
}

class GeneralizedVermaModuleCharacters {
public:
  List<Matrix<Rational> > linearOperators;
  // The first k variables correspond to the Cartan of the smaller Lie algebra.
  // The next l variables correspond to the Cartan of the larger Lie algebra.
  // The last variable is the projectivization.
  List<Matrix<Rational> > linearOperatorsExtended;
  Vector<Rational> nonIntegralOriginModificationBasisChanged;
  std::fstream multiplicitiesMaxOutputReport2;
  Vectors<Rational> gModKNegativeWeights;
  Vectors<Rational> gModKNegativeWeightsBasisChanged;
  Matrix<Rational> preferredBasisChange;
  Matrix<Rational> preferredBasisChangeInverse;
  Vectors<Rational> preferredBasis;
  Cone preimageWeylChamberLargerAlgebra;
  Cone weylChamberSmallerAlgebra;
  Cone preimageWeylChamberSmallerAlgebra;
  Lattice extendedIntegralLatticeMatrixForm;
  List<QuasiPolynomial> quasiPolynomialsNonSubstituted;
  List<List<QuasiPolynomial> > quasiPolynomialsSubstituted;
  List<QuasiPolynomial> multiplicities;
  HomomorphismSemisimpleLieAlgebra homomorphism;
  // List<QuasiPolynomial> multiplicitiesExtremaCandidates;
  int UpperLimitChambersForDebugPurposes;
  int numberNonZeroMultiplicities;
  Selection parabolicLeviPartRootSpacesZeroStandsForSelected;
  Selection parabolicSelectionSmallerAlgebra;
  List<Rational> coefficients;
  Vectors<Rational> translations;
  Vectors<Rational> translationsProjectedBasisChanged;
  PartialFractions partialFractions;
  // List<Cone> allParamSubChambersRepetitionsAllowedConeForm;
  ConeCollection projectivizedParamComplex;
  ConeLatticeAndShiftMaxComputation maximumComputation;
  ConeCollection smallerAlgebraChamber;
  ConeCollection projectivizedChamber;
  std::stringstream log;
  WeylGroupData* weylSmaller;
  WeylGroupData* weylLarger;
  int computationPhase;
  int numberOfProcessedConesParameters;
  int numberOfProcessedExtremaEqualToOne;
  std::string computeMultiplicitiesLargerAlgebraHighestWeight(
    Vector<Rational>& highestWeightLargerAlgebraFundamentalCoordinates,
    Vector<Rational>& parabolicSelection
  );
  std::string checkMultiplicitiesVsOrbits();
  std::string elementToStringMultiplicitiesReport();
  void incrementComputation(Vector<Rational>& parabolicSelection);
  std::string prepareReport();
  GeneralizedVermaModuleCharacters();
  bool checkInitialization() const;
  std::string prepareReportOneCone(
    FormatExpressions& format, const Cone& cone
  );
  void getProjection(
    int indexOperator, const Vector<Rational>& input, Vector<Rational>& output
  );
  void splitByMultiplicityFreeWall(Cone& cone, ConeCollection& output);
  void inititializeMaximumComputation();
  void computeQPsFromChamberComplex();
  void getSubstitutionFromIndex(
    PolynomialSubstitution<Rational>& outputSub,
    Matrix<LargeInteger>& outputMat,
    LargeIntegerUnsigned& ouputDen,
    int index
  );
  void sortMultiplicities();
  void getSubstitutionFromNonParametricArray(
    Matrix<Rational>& output,
    Vector<Rational>& outputTranslation,
    Vectors<Rational>& nonParameters,
    int numberOfParameters
  );
  void initFromHomomorphism(
    Vector<Rational>& parabolicSelection,
    HomomorphismSemisimpleLieAlgebra& input
  );
  void transformToWeylProjectiveStep1();
  void transformToWeylProjectiveStep2();
  void transformToWeylProjective(
    int indexOperator,
    Vector<Rational>& startingNormal,
    Vector<Rational>& outputNormal
  );
};

template <class Coefficient>
class ElementUniversalEnvelopingOrdered;

template <class Coefficient>
class MonomialUniversalEnvelopingOrdered {
  void simplifyAccumulateInOutputNoOutputInit(
    ElementUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  MonomialUniversalEnvelopingOrdered(
    const MonomialUniversalEnvelopingOrdered& other
  );
public:
  SemisimpleLieAlgebraOrdered* owner;
  // std::string DebugString;
  std::string toString(FormatExpressions* format) const;
  // SelectedIndices gives the non-zero powers of the generators participating
  // in the monomial
  // powers gives the powers of the generators in the order specified in the
  // owner
  List<int> generatorsIndices;
  List<Coefficient> powers;
  Coefficient coefficient;
  void multiplyBy(
    const MonomialUniversalEnveloping<Coefficient>& other,
    ElementUniversalEnvelopingOrdered<Coefficient>& output
  );
  void multiplyByGeneratorPowerOnTheRight(
    int generatorIndex, const Coefficient& power
  );
  void multiplyByGeneratorPowerOnTheRight(int generatorIndex, int power);
  void multiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other);
  void makeZero(
    int numberOfVariables, SemisimpleLieAlgebraOrdered& inputOwner
  );
  void makeZero(
    const Coefficient& ringZero, SemisimpleLieAlgebraOrdered& inputOwner
  );
  bool modOutFDRelationsExperimental(
    const Vector<Rational>& highestWeightSimpleCoordinates,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void modOutVermaRelations(
    const List<Coefficient>* substitutionHiGoesToIthElement = 0,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void setNumberOfVariables(int newNumberOfVariables);
  unsigned int hashFunction() const;
  static inline unsigned int hashFunction(
    const MonomialUniversalEnvelopingOrdered<Coefficient>& input
  ) {
    return input.hashFunction();
  }
  bool getElementUniversalEnveloping(
    ElementUniversalEnveloping<Coefficient>& output,
    SemisimpleLieAlgebraOrdered& owner
  );
  bool isEqualToZero() const {
    return this->coefficient.isEqualToZero();
  }
  bool commutingLeftIndexAroundRightIndexAllowed(
    Coefficient& leftPower,
    int leftGeneratorIndex,
    Coefficient& rightPower,
    int rightGeneratorIndex
  );
  bool commutingRightIndexAroundLeftIndexAllowed(
    Coefficient& leftPower,
    int leftGeneratorIndex,
    Coefficient& rightPower,
    int rightGeneratorIndex
  );
  bool switchConsecutiveIndicesIfTheyCommute(
    int leftIndex,
    MonomialUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringZero = 0
  );
  void makeConstant(
    const Coefficient& inputConstant, SemisimpleLieAlgebraOrdered& inputOwner
  ) {
    this->generatorsIndices.size = 0;
    this->powers.size = 0;
    this->coefficient = inputConstant;
    this->owner = &inputOwner;
  }
  void simplify(
    ElementUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void commuteConsecutiveIndicesLeftIndexAroundRight(
    int index,
    ElementUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void commuteConsecutiveIndicesRightIndexAroundLeft(
    int index,
    ElementUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  MonomialUniversalEnvelopingOrdered() {
    this->owner = nullptr;
  }
  void substitutionCoefficients(
    PolynomialSubstitution<Rational>& substitution
  );
  bool operator==(const MonomialUniversalEnvelopingOrdered& other) const {
    if (this->owner != other.owner) {
      global.fatal
      << "Attempt to compare universal enveloping algebra "
      << "monomials with different owners. "
      << global.fatal;
    }
    return
    this->powers == other.powers &&
    this->generatorsIndices == other.generatorsIndices;
  }
  void operator*=(const MonomialUniversalEnvelopingOrdered& other);
  template <class OtherCoefficientType>
  void assignChangeCoefficientType(
    const MonomialUniversalEnvelopingOrdered<OtherCoefficientType>& other
  ) {
    this->coefficient = other.coefficient;
    this->generatorsIndices = other.generatorsIndices.size;
    this->powers.setSize(other.powers.size);
    for (int i = 0; i < this->powers.size; i ++) {
      this->powers[i] = other.powers[i];
    }
  }
  inline void operator=(const MonomialUniversalEnvelopingOrdered& other) {
    this->generatorsIndices = other.generatorsIndices;
    this->powers = other.powers;
    this->coefficient = other.coefficient;
    this->owner = other.owner;
  }
};

template <class Coefficient>
class ElementUniversalEnvelopingOrdered: public HashedList<
  MonomialUniversalEnvelopingOrdered<Coefficient>
> {
private:
  void addMonomialNoCleanUpZeroCoefficient(
    const MonomialUniversalEnvelopingOrdered<Coefficient>& input
  );
  void cleanUpZeroCoefficient();
  friend class MonomialUniversalEnvelopingOrdered<Coefficient>;
public:
  void toString(std::string& output, FormatExpressions* polynomialFormatLocal)
  const;
  std::string toString(FormatExpressions* format) const {
    std::string currentString;
    this->toString(currentString, format);
    return currentString;
  }
  bool needsParenthesisForMultiplication() const {
    return this->size > 1;
  }
  SemisimpleLieAlgebraOrdered* owner;
  void addMonomial(
    const MonomialUniversalEnvelopingOrdered<Coefficient>& input
  );
  void assignElementLieAlgebra(
    const ElementSemisimpleLieAlgebra<Rational>& input,
    const Coefficient& ringUnit,
    const Coefficient& ringZero,
    SemisimpleLieAlgebraOrdered& inputOwner
  );
  void makeZero(SemisimpleLieAlgebraOrdered& inputOwner);
  bool assignElementUniversalEnveloping(
    ElementUniversalEnveloping<Coefficient>& input,
    SemisimpleLieAlgebraOrdered& owner,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  bool assignMonomialUniversalEnveloping(
    MonomialUniversalEnveloping<Coefficient>& input,
    const Coefficient& inputCoefficient,
    SemisimpleLieAlgebraOrdered& owner,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  bool modOutFDRelationsExperimental(
    const Vector<Rational>& highestWeightSimpleCoordinates,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  bool isEqualToZero() const {
    return this->size == 0;
  }
  bool getElementUniversalEnveloping(
    ElementUniversalEnveloping<Coefficient>& output,
    SemisimpleLieAlgebra& inputOwner
  );
  bool getLieAlgebraElementIfPossible(
    ElementSemisimpleLieAlgebra<Rational>& output
  ) const;
  void substitutionCoefficients(
    PolynomialSubstitution<Rational>& substitution
  );
  void makeConstant(
    const Coefficient& coefficient, SemisimpleLieAlgebraOrdered& inputOwner
  ) {
    this->makeZero(inputOwner);
    MonomialUniversalEnvelopingOrdered<Coefficient> monomial;
    monomial.makeConstant(coefficient, inputOwner);
    this->addMonomial(monomial);
  }
  void simplify(
    const Coefficient& ringUnit = 1, const Coefficient& ringZero = 0
  );
  inline void multiplyBy(const ElementUniversalEnvelopingOrdered& other) {
    this->operator*=(other);
  }
  void modOutVermaRelations(
    const List<Coefficient>* substitutionHiGoesToIthElement = 0,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void modOutVermaRelationsOld(
    bool substitutionHighestWeightWithZeroes,
    const PolynomialSubstitution<Rational>& highestWeightSub,
    const Coefficient& ringUnit
  );
  template <class CoefficientTypeQuotientField>
  static void getBasisFromSpanOfElements(
    List<ElementUniversalEnvelopingOrdered<Coefficient> >& elements,
    Vectors<CoefficientTypeQuotientField>& outputCoordinates,
    List<ElementUniversalEnvelopingOrdered<Coefficient> >& outputBasis,
    const CoefficientTypeQuotientField& fieldUnit,
    const CoefficientTypeQuotientField& fieldZero
  );
  bool coordinatesInBasis(
    List<ElementUniversalEnvelopingOrdered<Coefficient> >& basis,
    Vector<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  ) const;
  static void getCoordinateFormOfSpanOfElements(
    List<ElementUniversalEnvelopingOrdered>& elements,
    Vectors<Polynomial<Coefficient> >& outputCoordinates,
    ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials
  );
  void raiseToPower(int power, const Coefficient& ringUnit);
  bool isPowerOfSingleGenerator() {
    if (this->size != 1) {
      return false;
    }
    MonomialUniversalEnvelopingOrdered<Coefficient>& monomial =
    this->objects[0];
    if (!monomial.coefficient.isEqualToOne()) {
      return false;
    }
    if (monomial.generatorsIndices.size != 1) {
      return false;
    }
    return true;
  }
  void actOnMe(
    const ElementSemisimpleLieAlgebra<Rational>& element,
    ElementUniversalEnvelopingOrdered& output
  );
  void lieBracketOnTheRight(
    const ElementUniversalEnvelopingOrdered<Coefficient>& right,
    ElementUniversalEnvelopingOrdered<Coefficient>& output
  );
  void lieBracketOnTheRight(
    const ElementSemisimpleLieAlgebra<Rational>& right,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  void operator=(const ElementUniversalEnvelopingOrdered& other) {
    this->::HashedList<MonomialUniversalEnvelopingOrdered<Coefficient> >::
    operator=(other);
    this->owner = other.owner;
  }
  template <class OtherCoefficientType>
  void assignChangeCoefficientType(
    const ElementUniversalEnvelopingOrdered<OtherCoefficientType>& other
  ) {
    this->makeZero(*other.owner);
    MonomialUniversalEnvelopingOrdered<Coefficient> monomial;
    this->reserve(other.size);
    for (int i = 0; i < other.size; i ++) {
      monomial.assignChangeCoefficientType(other[i]);
      this->addMonomial(monomial);
    }
  }
  void operator+=(const ElementUniversalEnvelopingOrdered& other);
  void operator+=(int other);
  void operator+=(const Rational& other);
  void operator-=(const ElementUniversalEnvelopingOrdered& other);
  void operator*=(const ElementUniversalEnvelopingOrdered& other);
  template <class SecondType>
  void operator/=(const SecondType& other);
  template <class otherType>
  void operator*=(const otherType& other);
  ElementUniversalEnvelopingOrdered() {
    this->owner = nullptr;
  }
  bool isProportionalTo(
    const ElementUniversalEnvelopingOrdered<Coefficient>& other,
    Coefficient& outputTimesMeEqualsOther,
    const Coefficient& ringZero
  ) const;
  ElementUniversalEnvelopingOrdered(
    const ElementUniversalEnvelopingOrdered& other
  ) {
    this->operator=(other);
  }
  void clearDenominators(
    Coefficient& outputWasMultipliedBy, const Coefficient& ringUnit
  ) {
    outputWasMultipliedBy = ringUnit;
    Coefficient currentCoefficient;
    for (int i = 0; i < this->size; i ++) {
      MonomialUniversalEnvelopingOrdered<Coefficient>& monomial =
      this->objects[i];
      monomial.coefficient.clearDenominators(currentCoefficient);
      for (int j = 0; j < this->size; j ++) {
        if (j != i) {
          this->objects[j].coefficient *= currentCoefficient;
        }
      }
      outputWasMultipliedBy *= currentCoefficient;
    }
  }
};

template <class Coefficient>
class ElementVermaModuleOrdered {
public:
  ElementUniversalEnvelopingOrdered<Coefficient> elementInternal;
  PolynomialSubstitution<Coefficient>
  substitutionNthElementIsImageNthCoordinateSimpleBasis;
  std::string toString(const FormatExpressions& format) const;
  bool isEqualToZero() const {
    return this->elementInternal.isEqualToZero();
  }
  bool needsParenthesisForMultiplication() const;
  void assignElementUniversalEnvelopingOrderedTimesHighestWeightVector(
    ElementUniversalEnvelopingOrdered<Coefficient>& input,
    const ElementVermaModuleOrdered<Coefficient>& ringZero,
    const Coefficient& ringUnit
  );
  void actOnMe(
    const ElementSemisimpleLieAlgebra<Rational>& actingElement,
    ElementVermaModuleOrdered<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  ) const;
  static void getBasisFromSpanOfElements(
    List<ElementVermaModuleOrdered<Coefficient> >& elements,
    Vectors<RationalFraction<Rational> >& outputCoordinates,
    List<ElementVermaModuleOrdered>& outputBasis,
    const RationalFraction<Rational>& rationalFractionOne,
    const RationalFraction<Rational>& rationalFractionZero
  );
  bool coordinatesInBasis(
    const List<ElementVermaModuleOrdered<Coefficient> >& basis,
    Vector<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  ) const;
  bool isProportionalTo(
    const ElementVermaModuleOrdered<Coefficient>& other,
    Coefficient& outputTimesMeEqualsOther,
    const Coefficient& ringZero
  ) const {
    return
    this->elementInternal.isProportionalTo(
      other.elementInternal, outputTimesMeEqualsOther, ringZero
    );
  }
  void makeZero(
    SemisimpleLieAlgebraOrdered& owner,
    PolynomialSubstitution<Rational>& incomingSubstitution
  ) {
    this->elementInternal.makeZero(owner);
    this->substitutionNthElementIsImageNthCoordinateSimpleBasis =
    incomingSubstitution;
  }
  template <class CoefficientTypeOther>
  void operator*=(const CoefficientTypeOther& inputConstant) {
    this->elementInternal.operator*=(inputConstant);
  }
  void multiplyOnTheLeft(
    const ElementSemisimpleLieAlgebra<Rational>& other,
    ElementVermaModuleOrdered<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  void clearDenominators(
    Coefficient& outputWasMultipliedBy, const Coefficient& ringUnit
  ) {
    this->elementInternal.clearDenominators(outputWasMultipliedBy, ringUnit);
  }
  void operator/=(const Coefficient& inputConstant) {
    this->elementInternal.operator/=(inputConstant);
  }
  void operator-=(const ElementVermaModuleOrdered& other) {
    this->elementInternal -= other.elementInternal;
  }
  void operator+=(const ElementVermaModuleOrdered& other) {
    this->elementInternal += other.elementInternal;
  }
  void operator=(const ElementVermaModuleOrdered& other) {
    this->elementInternal = other.elementInternal;
    this->substitutionNthElementIsImageNthCoordinateSimpleBasis =
    other.substitutionNthElementIsImageNthCoordinateSimpleBasis;
  }
};

template <class Coefficient>
Coefficient SemisimpleLieAlgebra::getKillingFormProductWRTLevi(
  const ElementSemisimpleLieAlgebra<Coefficient>& left,
  const ElementSemisimpleLieAlgebra<Coefficient>& right,
  const Selection& rootsNotInLevi
) {
  STACK_TRACE("SemisimpleLieAlgebra::getKillingFormProductWRTLevi");
  Coefficient result = 0;
  ElementSemisimpleLieAlgebra<Coefficient> adadAppliedToMonomial;
  ElementSemisimpleLieAlgebra<Coefficient> element;
  ChevalleyGenerator baseGen;
  Vector<Rational> rootsNotInLeviVectorForm = rootsNotInLevi;
  Vector<Rational> weight;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    weight = this->getWeightOfGenerator(i);
    if (weight.scalarEuclidean(rootsNotInLeviVectorForm) != 0) {
      continue;
    }
    baseGen.makeGenerator(*this, i);
    adadAppliedToMonomial.makeZero();
    adadAppliedToMonomial.addMonomial(baseGen, 1);
    this->lieBracket(right, adadAppliedToMonomial, element);
    this->lieBracket(left, element, adadAppliedToMonomial);
    result += adadAppliedToMonomial.getCoefficientOf(baseGen);
  }
  return result;
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::
commutingAnBtoBAnPlusLowerOrderAllowed(
  Coefficient& leftPower,
  int leftGeneratorIndex,
  Coefficient& rightPower,
  int rightGeneratorIndex
) {
  return
  this->commutingABntoBnAPlusLowerOrderAllowed(
    rightPower, rightGeneratorIndex, leftPower, leftGeneratorIndex
  );
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::
commutingABntoBnAPlusLowerOrderAllowed(
  Coefficient& leftPower,
  int leftGeneratorIndex,
  Coefficient& rightPower,
  int rightGeneratorIndex
) {
  int leftPowerSmall = 0;
  int rightPowerSmall = 0;
  if (!leftPower.isSmallInteger(&leftPowerSmall)) {
    return false;
  }
  if (leftPowerSmall < 0) {
    return false;
  }
  if (rightPower.isSmallInteger(&rightPowerSmall)) {
    if (rightPowerSmall >= 0) {
      return true;
    }
  }
  int totalPositiveRoots = this->getOwner().weylGroup.rootsOfBorel.size;
  int dimension = this->getOwner().weylGroup.cartanSymmetric.numberOfRows;
  if (
    rightGeneratorIndex >= totalPositiveRoots &&
    rightGeneratorIndex < totalPositiveRoots + dimension
  ) {
    return
    this->getOwner().lieBrackets.elements[leftGeneratorIndex][
      rightGeneratorIndex
    ].isEqualToZero();
  }
  return true;
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::
switchConsecutiveIndicesIfTheyCommute(int leftIndex) {
  if (leftIndex >= this->generatorsIndices.size - 1) {
    return false;
  }
  int leftGenerator = this->generatorsIndices[leftIndex];
  int rightGenerator = this->generatorsIndices[leftIndex + 1];
  if (
    !this->getOwner().lieBrackets.elements[leftGenerator][rightGenerator].
    isEqualToZero()
  ) {
    return false;
  }
  this->generatorsIndices.swapTwoIndices(leftIndex, leftIndex + 1);
  this->powers.swapTwoIndices(leftIndex, leftIndex + 1);
  this->simplifyEqualConsecutiveGenerators(leftIndex - 1);
  return true;
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::commuteAnBtoBAnPlusLowerOrder(
  int indexA,
  ElementUniversalEnveloping<Coefficient>& output,
  const Coefficient& ringUnit
) {
  if (indexA == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Coefficient> monomial;
  monomial.makeOne(*this->owner);
  monomial.powers.setExpectedSize(this->generatorsIndices.size + 2);
  monomial.generatorsIndices.setExpectedSize(this->generatorsIndices.size + 2);
  monomial.powers.size = 0;
  monomial.generatorsIndices.size = 0;
  int rightGeneratorIndex = this->generatorsIndices[indexA + 1];
  int leftGeneratorIndex = this->generatorsIndices[indexA];
  Coefficient rightPower;
  Coefficient leftPower;
  rightPower = this->powers[indexA + 1];
  leftPower = this->powers[indexA];
  rightPower -= 1;
  int powerDrop = 0;
  Coefficient acquiredCoefficient;
  Coefficient incomingAcquiredCoefficient;
  acquiredCoefficient = ringUnit;
  for (int i = 0; i < indexA; i ++) {
    monomial.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices[i], this->powers[i]
    );
  }
  MonomialUniversalEnveloping<Coefficient> startMonomial;
  startMonomial = monomial;
  ElementSemisimpleLieAlgebra<Rational> adAToTheIthOfB;
  ElementSemisimpleLieAlgebra<Rational> aElement;
  adAToTheIthOfB.makeGenerator(rightGeneratorIndex, *this->owner);
  aElement.makeGenerator(leftGeneratorIndex, *this->owner);
  // Formula realized:
  // a^n b =\sum_{i = 0}^\infty \binom{n}{i} (\ad a)^i (b)a^{n-i}
  // Proof (Dixmier): let L_x stand for left multiplication by x and R_x stand
  // for right multiplication.
  // Then L_x and R_x commute and L_x-R_x =ad_x, i.e.
  // (L_a)^n =(R_a +ad_a)^n.
  do {
    for (int i = 0; i < adAToTheIthOfB.size(); i ++) {
      int newGeneratorIndex = adAToTheIthOfB[i].generatorIndex;
      monomial = startMonomial;
      incomingAcquiredCoefficient = acquiredCoefficient;
      incomingAcquiredCoefficient *= adAToTheIthOfB.coefficients[i];
      monomial.multiplyByGeneratorPowerOnTheRight(newGeneratorIndex, ringUnit);
      monomial.multiplyByGeneratorPowerOnTheRight(
        leftGeneratorIndex, leftPower
      );
      monomial.multiplyByGeneratorPowerOnTheRight(
        rightGeneratorIndex, rightPower
      );
      for (int i = indexA + 2; i < this->generatorsIndices.size; i ++) {
        monomial.multiplyByGeneratorPowerOnTheRight(
          this->generatorsIndices[i], this->powers[i]
        );
      }
      output.addMonomial(monomial, incomingAcquiredCoefficient);
    }
    powerDrop ++;
    acquiredCoefficient *= leftPower;
    acquiredCoefficient /= powerDrop;
    leftPower -= 1;
    this->getOwner().lieBracket(aElement, adAToTheIthOfB, adAToTheIthOfB);
  } while (
    !adAToTheIthOfB.isEqualToZero() && !acquiredCoefficient.isEqualToZero()
  );
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::commuteABntoBnAPlusLowerOrder(
  int index,
  ElementUniversalEnveloping<Coefficient>& output,
  const Coefficient& ringUnit
) {
  if (index == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Coefficient> monomial;
  monomial.makeOne(*this->owner);
  monomial.powers.setExpectedSize(this->generatorsIndices.size + 2);
  monomial.generatorsIndices.setExpectedSize(this->generatorsIndices.size + 2);
  monomial.powers.size = 0;
  monomial.generatorsIndices.size = 0;
  int rightGeneratorIndex = this->generatorsIndices[index + 1];
  int leftGeneratorIndex = this->generatorsIndices[index];
  Coefficient rightPower;
  Coefficient leftPower;
  rightPower = this->powers[index + 1];
  leftPower = this->powers[index];
  leftPower -= 1;
  int powerDrop = 0;
  Coefficient acquiredCoefficient;
  Coefficient incomingAcquiredCoefficient;
  acquiredCoefficient = ringUnit;
  for (int i = 0; i < index; i ++) {
    monomial.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices[i], this->powers[i]
    );
  }
  monomial.multiplyByGeneratorPowerOnTheRight(
    this->generatorsIndices[index], leftPower
  );
  MonomialUniversalEnveloping<Coefficient> startMonomial;
  startMonomial = monomial;
  ElementSemisimpleLieAlgebra<Rational> adResult;
  ElementSemisimpleLieAlgebra<Rational> element;
  ElementSemisimpleLieAlgebra<Rational> rightGeneratorElement;
  adResult.makeGenerator(leftGeneratorIndex, *this->owner);
  rightGeneratorElement.makeGenerator(rightGeneratorIndex, *this->owner);
  // Formula realized:
  // a b^n  &= &\sum_{i = 0}^\infty b^{n-i}(-\ad b)^i (a) \binom{n}{i} .
  // Proof (Dixmier): let L_x stand for left multiplication by x and R_x stand
  // for right multiplication.
  // Then L_x and R_x commute and L_x-R_x =ad_x, i.e.
  // (L_b-ad_b)^n =R_b^n.
  do {
    for (int i = 0; i < adResult.size(); i ++) {
      int newGeneratorIndex = adResult[i].generatorIndex;
      monomial = startMonomial;
      monomial.multiplyByGeneratorPowerOnTheRight(
        rightGeneratorIndex, rightPower
      );
      incomingAcquiredCoefficient = acquiredCoefficient;
      incomingAcquiredCoefficient *= adResult.coefficients[i];
      monomial.multiplyByGeneratorPowerOnTheRight(newGeneratorIndex, ringUnit);
      for (int i = index + 2; i < this->generatorsIndices.size; i ++) {
        monomial.multiplyByGeneratorPowerOnTheRight(
          this->generatorsIndices[i], this->powers[i]
        );
      }
      output.addMonomial(monomial, incomingAcquiredCoefficient);
    }
    acquiredCoefficient *= rightPower;
    rightPower -= 1;
    this->getOwner().lieBracket(adResult, rightGeneratorElement, element);
    adResult = element;
    powerDrop ++;
    acquiredCoefficient /= powerDrop;
  } while (!adResult.isEqualToZero() && !acquiredCoefficient.isEqualToZero());
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::
highestWeightTransposeAntiAutomorphismBilinearForm(
  const MonomialUniversalEnveloping<Coefficient>& right,
  Coefficient& output,
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  std::stringstream* logStream
) {
  ElementUniversalEnveloping<Coefficient> element1;
  ElementUniversalEnveloping<Coefficient> element2;
  element1.makeZero(*this->owner);
  element1.addMonomial(*this, ringUnit);
  element2.makeZero(*this->owner);
  element2.addMonomial(right, ringUnit);
  return
  element1.highestWeightTransposeAntiAutomorphismBilinearForm(
    element2,
    output,
    substitutionHiGoesToIthElement,
    ringUnit,
    ringZero,
    logStream
  );
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::getElementAdjointRepresentation(
  const ElementUniversalEnveloping<Coefficient>& element,
  Matrix<Coefficient>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("SemisimpleLieAlgebra::getElementAdjointRepresentation");
  int numberOfGenerators = this->getNumberOfGenerators();
  output.makeZeroMatrix(numberOfGenerators, 0);
  Coefficient one;
  one = 1;
  ElementUniversalEnveloping<Coefficient> basisElement;
  ElementUniversalEnveloping<Coefficient>
  actionOnBasisElementUniversalEnveloping;
  ElementSemisimpleLieAlgebra<Coefficient> actionOnBasisElement;
  for (int i = 0; i < numberOfGenerators; i ++) {
    basisElement.makeOneGenerator(i, *this, one);
    if (
      !element.adjointRepresentationAction(
        basisElement, actionOnBasisElementUniversalEnveloping
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Failed to extract action of "
        << element.toString()
        << " on "
        << basisElement.toString();
      }
      return false;
    }
    if (
      !actionOnBasisElementUniversalEnveloping.isLieAlgebraElement(
        actionOnBasisElement
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "<hr>While computing the ad-action of "
        << element.toString()
        << ": element "
        << actionOnBasisElementUniversalEnveloping.toString()
        << " is not in the semisimple Lie algebra.";
      }
      return false;
    }
    for (int j = 0; j < actionOnBasisElement.size(); j ++) {
      int basisIndex = actionOnBasisElement.monomials[j].generatorIndex;
      output(basisIndex, i) = actionOnBasisElement.coefficients[j];
    }
  }
  return true;
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::modOutVermaRelations(
  Coefficient& outputCoefficient,
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  int totalPositiveRoots = this->getOwner().getNumberOfPositiveRoots();
  int dimension = this->getOwner().getRank();
  outputCoefficient = ringUnit;
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int indexCurrentGenerator = this->generatorsIndices[i];
    if (indexCurrentGenerator >= totalPositiveRoots + dimension) {
      this->makeOne(*this->owner);
      outputCoefficient = ringZero;
      return;
    }
    if (indexCurrentGenerator < totalPositiveRoots) {
      return;
    }
    if (
      indexCurrentGenerator >= totalPositiveRoots &&
      indexCurrentGenerator < totalPositiveRoots + dimension
    ) {
      if (substitutionHiGoesToIthElement == 0) {
        this->makeOne(*this->owner);
        outputCoefficient = ringZero;
        return;
      }
      int degree = 0;
      if (!this->powers[i].isSmallInteger(&degree)) {
        return;
      }
      int hIndex = indexCurrentGenerator - totalPositiveRoots;
      Coefficient substitutedH;
      substitutedH = (*substitutionHiGoesToIthElement)[hIndex];
      MathRoutines::raiseToPower(substitutedH, degree, ringUnit);
      outputCoefficient *= substitutedH;
      this->generatorsIndices.size --;
      this->powers.size --;
    }
  }
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::setNumberOfVariables(
  int newNumberOfVariables
) {
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->powers.objects[i].setNumberOfVariablesSubstituteDeletedByOne(
      newNumberOfVariables
    );
  }
}

template <class Coefficient>
std::string MonomialUniversalEnveloping<Coefficient>::toString(
  FormatExpressions* format
) const {
  std::stringstream out;
  std::string currentString;
  if (this->owner == nullptr) {
    return "(Error:Programming:NonInitializedMonomial)";
  }
  if (this->generatorsIndices.size == 0) {
    return "1";
  }
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    Coefficient& power = this->powers[i];
    int index = this->generatorsIndices[i];
    currentString =
    this->getOwner().getStringFromChevalleyGenerator(index, format);
    out << currentString;
    if (!power.isEqualToOne()) {
      out << "^";
      out << "{";
      out << power.toString(format);
      out << "}";
    }
  }
  return out.str();
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::simplify(
  ElementUniversalEnveloping<Coefficient>& output, const Coefficient& ringUnit
) {
  output.makeZero(*this->owner);
  output.addOnTop(*this);
  this->simplifyAccumulateInOutputNoOutputInit(output, ringUnit);
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::
getElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& output,
  SemisimpleLieAlgebra& inputOwner
) {
  ElementUniversalEnveloping<Coefficient> accumulator;
  ElementUniversalEnveloping<Coefficient> element;
  accumulator.makeZero(inputOwner);
  for (int i = 0; i < this->size; i ++) {
    if (!this->objects[i].getElementUniversalEnveloping(element, owner)) {
      return false;
    } else {
      accumulator += element;
    }
  }
  output = accumulator;
  return true;
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::
getElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& output,
  SemisimpleLieAlgebraOrdered& inputOwner
) {
  ElementUniversalEnveloping<Coefficient> accumulator;
  ElementUniversalEnveloping<Coefficient> monomial;
  int index;
  int degree = 0;
  accumulator.makeConstant(this->coefficient, inputOwner);
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    if (this->powers[i].isSmallInteger(&degree)) {
      monomial.assignElementLieAlgebra(
        this->owner->elementOrder[this->generatorsIndices[i]],
        inputOwner,
        this->coefficient.getOne(),
        this->coefficient.GetZero()
      );
      monomial.raiseToPower(degree);
      accumulator *= monomial;
    } else {
      if (
        this->owner->elementOrder[this->generatorsIndices[i]].
        isCoefficientOneChevalleyGenerator()
      ) {
        monomial.makeOneGeneratorCoefficientOne(
          index,
          inputOwner,
          this->coefficient.getOne(),
          this->coefficient.GetZero()
        );
        monomial[0].powers[0] = this->powers[i];
        accumulator *= monomial;
      } else {
        return false;
      }
    }
  }
  output.operator=(accumulator);
  return true;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::setNumberOfVariables(
  int newNumberOfVariables
) {
  this->coefficient.setNumberOfVariablesSubstituteDeletedByOne(
    newNumberOfVariables
  );
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->powers[i].setNumberOfVariablesSubstituteDeletedByOne(
      newNumberOfVariables
    );
  }
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::coordinatesInBasis(
  List<ElementUniversalEnvelopingOrdered<Coefficient> >& basis,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) const {
  List<ElementUniversalEnvelopingOrdered<Coefficient> > bufferBasis;
  List<ElementUniversalEnvelopingOrdered<Coefficient> > elements;
  bufferBasis = basis;
  bufferBasis.addOnTop(*this);
  Vectors<Coefficient> coordinates;
  this->getBasisFromSpanOfElements(
    bufferBasis, coordinates, elements, ringUnit, ringZero
  );
  Vector<Coefficient> lastRoot;
  lastRoot = *coordinates.lastObject();
  coordinates.setSize(basis.size);
  return lastRoot.coordinatesInBasis(coordinates, output, ringUnit, ringZero);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::
getCoordinateFormOfSpanOfElements(
  List<ElementUniversalEnvelopingOrdered>& elements,
  Vectors<Polynomial<Coefficient> >& outputCoordinates,
  ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials
) {
  if (elements.size == 0) {
    return;
  }
  outputCorrespondingMonomials.makeZero(*elements[0].owner);
  for (int i = 0; i < elements.size; i ++) {
    for (int j = 0; j < elements[i].size; j ++) {
      outputCorrespondingMonomials.addOnTopNoRepetition(elements[i][j]);
    }
  }
  outputCoordinates.setSize(elements.size);
  Polynomial<Rational> zeroPoly;
  zeroPoly.makeZero();
  for (int i = 0; i < elements.size; i ++) {
    Vector<Polynomial<Coefficient> >& current = outputCoordinates[i];
    current.initializeFillInObject(
      outputCorrespondingMonomials.size, zeroPoly
    );
    ElementUniversalEnvelopingOrdered& currentElement = elements[i];
    for (int j = 0; j < currentElement.size; j ++) {
      MonomialUniversalEnvelopingOrdered<Coefficient>& monomial =
      currentElement[j];
      current.objects[outputCorrespondingMonomials.getIndex(monomial)] =
      monomial.coefficient;
    }
  }
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::substitutionCoefficients(
  PolynomialSubstitution<Rational>& substitution
) {
  if (substitution.size < 1) {
    return;
  }
  this->coefficient.substitute(substitution);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::substitutionCoefficients(
  PolynomialSubstitution<Rational>& substitution
) {
  ElementUniversalEnvelopingOrdered<Coefficient> endResult;
  MonomialUniversalEnvelopingOrdered<Coefficient> monomial;
  endResult.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    monomial = (*this)[i];
    monomial.substitutionCoefficients(substitution);
    endResult.addMonomial(monomial);
  }
  this->operator=(endResult);
}

template <class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::getBasisFromSpanOfElements(
  List<ElementVermaModuleOrdered>& elements,
  Vectors<RationalFraction<Rational> >& outputCoordinates,
  List<ElementVermaModuleOrdered>& outputBasis,
  const RationalFraction<Rational>& rationalFractionOne,
  const RationalFraction<Rational>& rationalFractionZero
) {
  List<ElementUniversalEnvelopingOrdered<Coefficient> > elementsUEform;
  elementsUEform.setSize(elements.size);
  for (int i = 0; i < elements.size; i ++) {
    elementsUEform[i] = elements[i].elementInternal;
  }
  List<ElementUniversalEnvelopingOrdered<Coefficient> >
  basisUniversalEnvelopingForm;
  ElementUniversalEnvelopingOrdered<Coefficient>::getBasisFromSpanOfElements(
    elementsUEform,
    outputCoordinates,
    basisUniversalEnvelopingForm,
    rationalFractionOne,
    rationalFractionZero,
    global
  );
  outputBasis.setSize(basisUniversalEnvelopingForm.size);
  for (int i = 0; i < basisUniversalEnvelopingForm.size; i ++) {
    outputBasis[i].elementInternal = basisUniversalEnvelopingForm[i];
    outputBasis[i].substitutionNthElementIsImageNthCoordinateSimpleBasis =
    elements[0].substitutionNthElementIsImageNthCoordinateSimpleBasis;
  }
}

template <class Coefficient>
bool ElementVermaModuleOrdered<Coefficient>::coordinatesInBasis(
  const List<ElementVermaModuleOrdered<Coefficient> >& basis,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) const {
  List<ElementUniversalEnvelopingOrdered<Coefficient> > elementsUEform;
  elementsUEform.setSize(basis.size);
  for (int i = 0; i < basis.size; i ++) {
    elementsUEform.objects[i] = basis.objects[i].elementInternal;
  }
  return
  this->elementInternal.coordinatesInBasis(
    elementsUEform, output, ringUnit, ringZero
  );
}

template <class Coefficient>
bool ElementVermaModuleOrdered<Coefficient>::needsParenthesisForMultiplication(
) const {
  return this->elementInternal.needsParenthesisForMultiplication();
}

template <class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::
assignElementUniversalEnvelopingOrderedTimesHighestWeightVector(
  ElementUniversalEnvelopingOrdered<Coefficient>& input,
  const ElementVermaModuleOrdered<Coefficient>& ringZero,
  const Coefficient& ringUnit
) {
  this->elementInternal.operator=(input);
  if (
    ringZero.substitutionNthElementIsImageNthCoordinateSimpleBasis.size != 3
  ) {
    global.fatal
    << "ringZero.substitutionNthElementIsImageNthCoordinateSimpleBasis.size "
    << "is not equal to 3 as expected. "
    << global.fatal;
  }
  this->substitutionNthElementIsImageNthCoordinateSimpleBasis =
  ringZero.substitutionNthElementIsImageNthCoordinateSimpleBasis;
  this->elementInternal.modOutVermaRelationsOld(
    false,
    this->substitutionNthElementIsImageNthCoordinateSimpleBasis,
    ringUnit
  );
}

template <class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::multiplyOnTheLeft(
  const ElementSemisimpleLieAlgebra<Rational>& other,
  ElementVermaModuleOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered<Coefficient> element;
  element.assignElementLieAlgebra(
    other, ringUnit, ringZero, *this->elementInternal.owner
  );
  element *= this->elementInternal;
  output.elementInternal = element;
  output.elementInternal.simplify(ringUnit, ringZero);
  output.elementInternal.modOutVermaRelationsOld(
    false,
    this->substitutionNthElementIsImageNthCoordinateSimpleBasis,
    ringUnit
  );
}

template <class Coefficient>
std::string ElementVermaModuleOrdered<Coefficient>::toString(
  const FormatExpressions& format
) const {
  std::stringstream out;
  std::string currentString =
  MathRoutines::toStringBrackets(this->elementInternal, format);
  if (currentString.size() > 1) {
    out << "(";
  }
  if (currentString != "1") {
    out << currentString;
  }
  if (currentString.size() > 1) {
    out << ")";
  }
  if (currentString != "0") {
    out << " v";
  }
  return out.str();
}

template <class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::actOnMe(
  const ElementSemisimpleLieAlgebra<Rational>& actingElement,
  ElementVermaModuleOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) const {
  ElementUniversalEnvelopingOrdered<Coefficient> element;
  element.assignElementLieAlgebra(
    actingElement, ringUnit, ringZero, *this->elementInternal.owner
  );
  element.lieBracketOnTheRight(this->elementInternal, output.elementInternal);
  output.elementInternal.simplify(ringUnit, ringZero);
  output.substitutionNthElementIsImageNthCoordinateSimpleBasis =
  this->substitutionNthElementIsImageNthCoordinateSimpleBasis;
  output.elementInternal.modOutVermaRelationsOld(
    false,
    this->substitutionNthElementIsImageNthCoordinateSimpleBasis,
    ringUnit
  );
}

template <class Coefficient>
template <class CoefficientTypeQuotientField>
void ElementUniversalEnvelopingOrdered<Coefficient>::getBasisFromSpanOfElements
(
  List<ElementUniversalEnvelopingOrdered<Coefficient> >& elements,
  Vectors<CoefficientTypeQuotientField>& outputCoordinates,
  List<ElementUniversalEnvelopingOrdered<Coefficient> >& outputBasis,
  const CoefficientTypeQuotientField& fieldUnit,
  const CoefficientTypeQuotientField& fieldZero
) {
  if (elements.size == 0) {
    return;
  }
  ElementUniversalEnvelopingOrdered<Coefficient> outputCorrespondingMonomials;
  outputCorrespondingMonomials.makeZero(*elements[0].owner);
  Vectors<CoefficientTypeQuotientField> outputCoordinatesBeforeReduction;
  for (int i = 0; i < elements.size; i ++) {
    for (int j = 0; j < elements[i].size; j ++) {
      outputCorrespondingMonomials.addOnTopNoRepetition(elements[i][j]);
    }
  }
  outputCoordinatesBeforeReduction.setSize(elements.size);
  for (int i = 0; i < elements.size; i ++) {
    Vector<CoefficientTypeQuotientField>& currentList =
    outputCoordinatesBeforeReduction[i];
    currentList.makeZero(outputCorrespondingMonomials.size, fieldZero);
    ElementUniversalEnvelopingOrdered<Coefficient>& currentElement =
    elements[i];
    for (int j = 0; j < currentElement.size; j ++) {
      MonomialUniversalEnvelopingOrdered<Coefficient>& monomial =
      currentElement[j];
      currentList.objects[outputCorrespondingMonomials.getIndex(monomial)] =
      monomial.coefficient;
    }
  }
  outputBasis.size = 0;
  outputBasis.reserve(elements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordForm;
  basisCoordForm.reserve(elements.size);
  Selection selectedBasis;
  outputCoordinatesBeforeReduction.SelectABasis(
    basisCoordForm, fieldZero, selectedBasis
  );
  for (int i = 0; i < selectedBasis.cardinalitySelection; i ++) {
    outputBasis.addOnTop(elements[selectedBasis.elements[i]]);
  }
  Matrix<Coefficient> bufferMatrix;
  Vectors<Coefficient> bufferVectors;
  outputCoordinatesBeforeReduction.coordinatesInBasis(
    basisCoordForm,
    outputCoordinates,
    bufferVectors,
    bufferMatrix,
    fieldUnit,
    fieldZero
  );
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::simplify(
  const Coefficient& ringUnit, const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered buffer;
  ElementUniversalEnvelopingOrdered output;
  output.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    this->objects[i].simplify(buffer, ringUnit, ringZero);
    output += buffer;
  }
  *this = output;
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::makeZero(
  SemisimpleLieAlgebraOrdered& inputOwner
) {
  this->clear();
  this->owner = &inputOwner;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::simplify(
  ElementUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  output.makeZero(*this->owner);
  output.addOnTop(*this);
  this->simplifyAccumulateInOutputNoOutputInit(output, ringUnit, ringZero);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::cleanUpZeroCoefficient() {
  for (int i = 0; i < this->size; i ++) {
    if (this->objects[i].coefficient.isEqualToZero()) {
      this->removeIndexSwapWithLast(i);
      i --;
    }
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator+=(
  const ElementUniversalEnvelopingOrdered<Coefficient>& other
) {
  this->reserve(this->size + other.size);
  for (int i = 0; i < other.size; i ++) {
    this->addMonomialNoCleanUpZeroCoefficient(other[i]);
  }
  this->cleanUpZeroCoefficient();
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator-=(
  const ElementUniversalEnvelopingOrdered<Coefficient>& other
) {
  this->reserve(this->size + other.size);
  MonomialUniversalEnvelopingOrdered<Coefficient> monomial;
  for (int i = 0; i < other.size; i ++) {
    monomial = other.objects[i];
    monomial.coefficient *= - 1;
    this->addMonomialNoCleanUpZeroCoefficient(monomial);
  }
  this->cleanUpZeroCoefficient();
}

template <class Coefficient>
template <class otherType>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator*=(
  const otherType& other
) {
  if (other.isEqualToZero()) {
    this->makeZero(*this->owner);
    return;
  }
  for (int i = 0; i < this->size; i ++) {
    this->objects[i].coefficient.operator*=(other);
  }
}

template <class Coefficient>
template <class SecondType>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator/=(
  const SecondType& other
) {
  for (int i = 0; i < this->size; i ++) {
    this->objects[i].coefficient.operator/=(other);
  }
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::multiplyByNoSimplify(
  const MonomialUniversalEnvelopingOrdered& other
) {
  if (this == &other) {
    global.fatal
    << "Output not allowed to be equal to this object. "
    << global.fatal;
  }
  this->generatorsIndices.reserve(
    other.generatorsIndices.size + this->generatorsIndices.size
  );
  this->powers.reserve(
    other.generatorsIndices.size + this->generatorsIndices.size
  );
  this - coefficient.multiplyBy(other.coefficient);
  if (other.generatorsIndices.size == 0) {
    return;
  }
  int firstIndex = other.generatorsIndices[0];
  int i = 0;
  if (this->generatorsIndices.size > 0) {
    if (firstIndex == (*this->generatorsIndices.lastObject())) {
      *this->powers.lastObject() += other.powers[0];
      i = 1;
    }
  }
  for (; i < other.generatorsIndices.size; i ++) {
    this->powers.addOnTop(other.powers[i]);
    this->generatorsIndices.addOnTop(other.generatorsIndices[i]);
  }
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::isProportionalTo(
  const ElementUniversalEnvelopingOrdered<Coefficient>& other,
  Coefficient& outputTimesMeEqualsOther,
  const Coefficient& ringZero
) const {
  if (this->isEqualToZero()) {
    if (other.isEqualToZero()) {
      return true;
    }
    return false;
  }
  if (other.isEqualToZero()) {
    outputTimesMeEqualsOther = ringZero;
    return true;
  }
  if (other.size != this->size) {
    return false;
  }
  MonomialUniversalEnvelopingOrdered<Coefficient>& monomial = (*this)[0];
  int index = other.getIndex(monomial);
  if (index == - 1) {
    return false;
  }
  MonomialUniversalEnvelopingOrdered<Coefficient>& otherMonomial =
  other[index];
  outputTimesMeEqualsOther = otherMonomial.coefficient;
  outputTimesMeEqualsOther /= monomial.coefficient;
  ElementUniversalEnvelopingOrdered<Coefficient> element;
  element = *this;
  element *= outputTimesMeEqualsOther;
  element -= other;
  return element.isEqualToZero();
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::operator*=(
  const MonomialUniversalEnvelopingOrdered& other
) {
  if (this == &other) {
    global.fatal << "Output and input are the same. " << global.fatal;
  }
  this->coefficient *= other.coefficient;
  for (int i = 0; i < other.generatorsIndices.size; i ++) {
    this->multiplyByGeneratorPowerOnTheRight(
      other.generatorsIndices[i], other.powers[i]
    );
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator*=(
  const ElementUniversalEnvelopingOrdered<Coefficient>& other
) {
  ElementUniversalEnvelopingOrdered output;
  output.makeZero(*this->owner);
  output.reserve(this->size * other.size);
  MonomialUniversalEnvelopingOrdered<Coefficient> monomial;
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < other.size; j ++) {
      monomial = (*this)[i];
      monomial *= other[j];
      output.addMonomial(monomial);
    }
  }
  *this = output;
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::
addMonomialNoCleanUpZeroCoefficient(
  const MonomialUniversalEnvelopingOrdered<Coefficient>& input
) {
  int index = this->getIndex(input);
  if (index == - 1) {
    this->addOnTop(input);
  } else {
    this->objects[index].coefficient += input.coefficient;
  }
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::
simplifyAccumulateInOutputNoOutputInit(
  ElementUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  int indexlowestNonSimplified = 0;
  ElementUniversalEnvelopingOrdered<Coefficient> buffer2;
  MonomialUniversalEnvelopingOrdered<Coefficient> monomial;
  // simplified order is descending order
  while (indexlowestNonSimplified < output.size) {
    bool reductionOccurred = false;
    if (output[indexlowestNonSimplified].coefficient.isEqualToZero()) {
      reductionOccurred = true;
    } else {
      for (
        int i = 0; i < output[indexlowestNonSimplified].generatorsIndices.
        size -
        1; i ++
      ) {
        if (
          output[indexlowestNonSimplified].generatorsIndices[i] >
          output[indexlowestNonSimplified].generatorsIndices[i + 1]
        ) {
          if (
            output[indexlowestNonSimplified].
            switchConsecutiveIndicesIfTheyCommute(i, monomial, ringZero)
          ) {
            output.addMonomialNoCleanUpZeroCoefficient(monomial);
            reductionOccurred = true;
            break;
          }
          if (
            this->commutingRightIndexAroundLeftIndexAllowed(
              output[indexlowestNonSimplified].powers[i],
              output[indexlowestNonSimplified].generatorsIndices[i],
              output[indexlowestNonSimplified].powers[i + 1],
              output[indexlowestNonSimplified].generatorsIndices[i + 1]
            )
          ) {
            output[indexlowestNonSimplified].
            commuteConsecutiveIndicesRightIndexAroundLeft(
              i, buffer2, ringUnit, ringZero
            );
            for (int j = 0; j < buffer2.size; j ++) {
              output.addMonomialNoCleanUpZeroCoefficient(buffer2[j]);
            }
            reductionOccurred = true;
            break;
          }
          if (
            this->commutingLeftIndexAroundRightIndexAllowed(
              output[indexlowestNonSimplified].powers[i],
              output[indexlowestNonSimplified].generatorsIndices[i],
              output[indexlowestNonSimplified].powers[i + 1],
              output[indexlowestNonSimplified].generatorsIndices[i + 1]
            )
          ) {
            output[indexlowestNonSimplified].
            commuteConsecutiveIndicesLeftIndexAroundRight(
              i, buffer2, ringUnit, ringZero
            );
            for (int j = 0; j < buffer2.size; j ++) {
              output.addMonomialNoCleanUpZeroCoefficient(buffer2[j]);
            }
            reductionOccurred = true;
            break;
          }
        }
      }
    }
    if (reductionOccurred) {
      output.removeIndexSwapWithLast(indexlowestNonSimplified);
    } else {
      indexlowestNonSimplified ++;
    }
  }
  output.cleanUpZeroCoefficient();
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::
switchConsecutiveIndicesIfTheyCommute(
  int leftIndex,
  MonomialUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringZero
) {
  if (leftIndex >= this->generatorsIndices.size - 1) {
    return false;
  }
  int leftGeneratorIndex = this->generatorsIndices[leftIndex];
  int rightGeneratorIndex = this->generatorsIndices[leftIndex + 1];
  ElementSemisimpleLieAlgebra<Rational> element;
  this->owner->ownerSemisimpleLieAlgebra->lieBracket(
    this->owner->elementOrder[leftGeneratorIndex],
    this->owner->elementOrder[rightGeneratorIndex],
    element
  );
  if (element.isEqualToZero()) {
    output.generatorsIndices.reserve(this->generatorsIndices.size);
    output.powers.reserve(this->generatorsIndices.size);
    output.makeZero(ringZero, *this->owner);
    output.coefficient = this->coefficient;
    for (int i = 0; i < leftIndex; i ++) {
      output.multiplyByGeneratorPowerOnTheRight(
        this->generatorsIndices[i], this->powers[i]
      );
    }
    output.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices[leftIndex + 1], this->powers[leftIndex + 1]
    );
    output.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices[leftIndex], this->powers[leftIndex]
    );
    for (int i = leftIndex + 2; i < this->generatorsIndices.size; i ++) {
      output.multiplyByGeneratorPowerOnTheRight(
        this->generatorsIndices[i], this->powers[i]
      );
    }
    return true;
  }
  return false;
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::
commutingRightIndexAroundLeftIndexAllowed(
  Coefficient& leftPower,
  int leftGeneratorIndex,
  Coefficient& rightPower,
  int rightGeneratorIndex
) {
  return
  this->commutingLeftIndexAroundRightIndexAllowed(
    rightPower, rightGeneratorIndex, leftPower, leftGeneratorIndex
  );
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::
commuteConsecutiveIndicesRightIndexAroundLeft(
  int index,
  ElementUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  if (index == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnvelopingOrdered monomial;
  monomial.makeZero(ringZero, *this->owner);
  monomial.powers.reserve(this->generatorsIndices.size + 2);
  monomial.generatorsIndices.reserve(this->generatorsIndices.size + 2);
  monomial.powers.size = 0;
  monomial.generatorsIndices.size = 0;
  int rightGeneratorIndex = this->generatorsIndices.objects[index + 1];
  int leftGeneratorIndex = this->generatorsIndices.objects[index];
  Coefficient rightPower;
  Coefficient leftPower;
  rightPower = this->powers.objects[index + 1];
  leftPower = this->powers.objects[index];
  rightPower -= 1;
  int powerDrop = 0;
  Coefficient acquiredCoefficient;
  acquiredCoefficient = this->coefficient;
  monomial.coefficient = this->coefficient;
  for (int i = 0; i < index; i ++) {
    monomial.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices[i], this->powers[i]
    );
  }
  MonomialUniversalEnvelopingOrdered startMonomial;
  startMonomial = monomial;
  ElementSemisimpleLieAlgebra<Rational> adResult;
  ElementSemisimpleLieAlgebra<Rational> element;
  ElementSemisimpleLieAlgebra<Rational> leftElement;
  this->owner->assignGeneratorCoefficientOne(rightGeneratorIndex, adResult);
  this->owner->assignGeneratorCoefficientOne(leftGeneratorIndex, leftElement);
  Vector<Rational> coefficients;
  do {
    this->owner->getLinearCombinationFrom(adResult, coefficients);
    for (int i = 0; i < coefficients.size; i ++) {
      if (coefficients[i] != 0) {
        int newGeneratorIndex = i;
        monomial = startMonomial;
        monomial.coefficient = acquiredCoefficient;
        monomial.coefficient *= coefficients[i];
        monomial.multiplyByGeneratorPowerOnTheRight(
          newGeneratorIndex, ringUnit
        );
        monomial.multiplyByGeneratorPowerOnTheRight(
          leftGeneratorIndex, leftPower
        );
        monomial.multiplyByGeneratorPowerOnTheRight(
          rightGeneratorIndex, rightPower
        );
        for (int i = index + 2; i < this->generatorsIndices.size; i ++) {
          monomial.multiplyByGeneratorPowerOnTheRight(
            this->generatorsIndices.objects[i], this->powers.objects[i]
          );
        }
        output.addOnTop(monomial);
      }
    }
    acquiredCoefficient *= leftPower;
    leftPower -= 1;
    this->owner->ownerSemisimpleLieAlgebra->lieBracket(
      leftElement, adResult, element
    );
    adResult = element;
    powerDrop ++;
    acquiredCoefficient /= powerDrop;
  } while (!adResult.isEqualToZero() && !acquiredCoefficient.isEqualToZero());
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::
commuteConsecutiveIndicesLeftIndexAroundRight(
  int index,
  ElementUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  if (index == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnvelopingOrdered monomial;
  monomial.makeZero(ringZero, *this->owner);
  monomial.powers.setExpectedSize(this->generatorsIndices.size + 2);
  monomial.generatorsIndices.setExpectedSize(this->generatorsIndices.size + 2);
  monomial.powers.size = 0;
  monomial.generatorsIndices.size = 0;
  int rightGeneratorIndex = this->generatorsIndices.objects[index + 1];
  int leftGeneratorIndex = this->generatorsIndices.objects[index];
  Coefficient rightPower;
  Coefficient leftPower;
  rightPower = this->powers.objects[index + 1];
  leftPower = this->powers.objects[index];
  leftPower -= 1;
  int powerDrop = 0;
  Coefficient acquiredCoefficient;
  acquiredCoefficient = this->coefficient;
  monomial.coefficient = this->coefficient;
  for (int i = 0; i < index; i ++) {
    monomial.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices.objects[i], this->powers.objects[i]
    );
  }
  monomial.multiplyByGeneratorPowerOnTheRight(
    this->generatorsIndices.objects[index], leftPower
  );
  MonomialUniversalEnvelopingOrdered startMonomial;
  MonomialUniversalEnvelopingOrdered monomial2;
  startMonomial = monomial;
  ElementSemisimpleLieAlgebra<Rational> adResult;
  ElementSemisimpleLieAlgebra<Rational> element;
  ElementSemisimpleLieAlgebra<Rational> temprightElement;
  this->owner->assignGeneratorCoefficientOne(leftGeneratorIndex, adResult);
  this->owner->assignGeneratorCoefficientOne(
    rightGeneratorIndex, temprightElement
  );
  Vector<Rational> coefficients;
  do {
    this->owner->getLinearCombinationFrom(adResult, coefficients);
    for (int i = 0; i < coefficients.size; i ++) {
      if (coefficients[i] != 0) {
        int newGeneratorIndex = i;
        monomial = startMonomial;
        monomial.multiplyByGeneratorPowerOnTheRight(
          rightGeneratorIndex, rightPower
        );
        monomial.coefficient = acquiredCoefficient;
        monomial.coefficient *= coefficients[i];
        monomial.multiplyByGeneratorPowerOnTheRight(
          newGeneratorIndex, ringUnit
        );
        for (int i = index + 2; i < this->generatorsIndices.size; i ++) {
          monomial.multiplyByGeneratorPowerOnTheRight(
            this->generatorsIndices[i], this->powers[i]
          );
        }
        output.addOnTop(monomial);
      }
    }
    acquiredCoefficient *= rightPower;
    rightPower -= 1;
    this->owner->ownerSemisimpleLieAlgebra->lieBracket(
      adResult, temprightElement, element
    );
    adResult = element;
    powerDrop ++;
    acquiredCoefficient /= powerDrop;
  } while (!adResult.isEqualToZero() && !acquiredCoefficient.isEqualToZero());
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::
commutingLeftIndexAroundRightIndexAllowed(
  Coefficient& leftPower,
  int leftGeneratorIndex,
  Coefficient& rightPower,
  int rightGeneratorIndex
) {
  int tempInt;
  if (leftPower.isSmallInteger(&tempInt)) {
    if (rightPower.isSmallInteger(&tempInt)) {
      return true;
    }
    int totalPositiveRoots =
    this->owner->ownerSemisimpleLieAlgebra->weylGroup.rootsOfBorel.size;
    int dimension =
    this->owner->ownerSemisimpleLieAlgebra->weylGroup.cartanSymmetric.
    numberOfRows;
    if (
      rightGeneratorIndex >= totalPositiveRoots &&
      rightGeneratorIndex < totalPositiveRoots + dimension
    ) {
      ElementSemisimpleLieAlgebra<Rational> element;
      this->owner->ownerSemisimpleLieAlgebra->lieBracket(
        this->owner->elementOrder[leftGeneratorIndex],
        this->owner->elementOrder[rightGeneratorIndex],
        element
      );
      if (element.isEqualToZero()) {
        return true;
      } else {
        return false;
      }
    } else {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::makeZero(
  int numberOfVariables, SemisimpleLieAlgebraOrdered& inputOwner
) {
  this->coefficient.makeZero(numberOfVariables);
  this->owner = &inputOwner;
  this->generatorsIndices.size = 0;
  this->powers.size = 0;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::makeZero(
  const Coefficient& ringZero, SemisimpleLieAlgebraOrdered& inputOwner
) {
  this->coefficient = ringZero;
  this->owner = &inputOwner;
  this->generatorsIndices.size = 0;
  this->powers.size = 0;
}

template <class Coefficient>
unsigned int MonomialUniversalEnvelopingOrdered<Coefficient>::hashFunction()
const {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < this->powers.size; i ++) {
    result +=
    HashConstants::getConstantIncrementCounter(j) *
    this->generatorsIndices.objects[i];
  }
  return result;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::
multiplyByGeneratorPowerOnTheRight(int generatorIndex, int power) {
  if (power == 0) {
    return;
  }
  Polynomial<Rational> polynomial;
  polynomial.makeConstant(power);
  this->multiplyByGeneratorPowerOnTheRight(generatorIndex, polynomial);
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::
multiplyByGeneratorPowerOnTheRight(
  int generatorIndex, const Coefficient& power
) {
  if (power.isEqualToZero()) {
    return;
  }
  if (this->generatorsIndices.size > 0) {
    if (*this->generatorsIndices.lastObject() == generatorIndex) {
      (*this->powers.lastObject()) += power;
      return;
    }
  }
  this->powers.addOnTop(power);
  this->generatorsIndices.addOnTop(generatorIndex);
}

template <class Coefficient>
std::string MonomialUniversalEnvelopingOrdered<Coefficient>::toString(
  FormatExpressions* format
) const {
  if (this->owner == nullptr) {
    return "Faulty monomial: non-initialized owner. ";
  }
  if (this->isEqualToZero()) {
    return "0";
  }
  std::stringstream out;
  std::string currentString;
  if (this->generatorsIndices.size > 0) {
    currentString = MathRoutines::toStringBrackets(this->coefficient, format);
    if (currentString == "1") {
      currentString = "";
    }
    if (currentString == "- 1" || currentString == "-1") {
      currentString = "-";
    }
  } else {
    currentString = this->coefficient.toString(format);
  }
  out << currentString;
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    Coefficient& power = this->powers[i];
    int index = this->generatorsIndices[i];
    bool usebrackets = false;
    currentString =
    this->owner->ownerSemisimpleLieAlgebra->getStringFromChevalleyGenerator(
      index, format
    );
    if (usebrackets) {
      out << "(";
    }
    out << currentString;
    if (usebrackets) {
      out << ")";
    }
    currentString = power.toString(format);
    if (currentString != "1") {
      out << "^";
      // if (useLatex)
      out << "{";
      out << currentString;
      // if (useLatex)
      out << "}";
    }
    // if (power>1)
    //  out << ")";
  }
  return out.str();
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::toString(
  std::string& output, FormatExpressions* polynomialFormatLocal
) const {
  std::stringstream out;
  std::string currentString;
  if (this->size == 0) {
    out << "0";
  }
  int indexCharacterAtLastLineBreak = 0;
  for (int i = 0; i < this->size; i ++) {
    MonomialUniversalEnvelopingOrdered<Coefficient>& current =
    this->objects[i];
    currentString = current.toString(polynomialFormatLocal);
    if (i != 0) {
      if (currentString.size() > 0) {
        if (currentString[0] != '-') {
          out << '+';
        }
      }
    }
    out << currentString;
    if ((static_cast<int>(out.tellp())) - indexCharacterAtLastLineBreak > 150) {
      indexCharacterAtLastLineBreak = out.tellp();
      out << "\\\\&&";
    }
  }
  output = out.str();
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::
assignElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& input,
  SemisimpleLieAlgebraOrdered& owner,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered<Coefficient> element;
  this->makeZero(owner);
  for (int i = 0; i < input.size; i ++) {
    if (
      !element.assignMonomialUniversalEnveloping(
        input.objects[i], input.coefficients[i], owner, ringUnit, ringZero
      )
    ) {
      return false;
    }
    this->operator+=(element);
  }
  this->simplify(ringUnit, ringZero);
  return true;
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::
assignMonomialUniversalEnveloping(
  MonomialUniversalEnveloping<Coefficient>& input,
  const Coefficient& inputCoefficient,
  SemisimpleLieAlgebraOrdered& owner,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered monomial;
  ElementSemisimpleLieAlgebra<Rational> element;
  Coefficient coefficient;
  coefficient = inputCoefficient;
  this->makeConstant(coefficient, owner);
  for (int i = 0; i < input.generatorsIndices.size; i ++) {
    int power;
    bool isASmallInteger = input.powers.objects[i].isSmallInteger(&power);
    if (isASmallInteger) {
      element.makeGenerator(
        input.generatorsIndices.objects[i], *input.owners, input.indexInOwners
      );
      monomial.assignElementLieAlgebra(element, ringUnit, ringZero, owner);
      monomial.raiseToPower(power, ringUnit);
    } else {
      return false;
    }
    this->multiplyBy(monomial);
  }
  return true;
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::assignElementLieAlgebra(
  const ElementSemisimpleLieAlgebra<Rational>& input,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  SemisimpleLieAlgebraOrdered& inputOwner
) {
  this->makeZero(inputOwner);
  Vector<Rational> elementRootForm;
  input.toVectorNegativeRootSpacesFirst(elementRootForm);
  inputOwner.chevalleyGeneratorsInCurrentCoordinates.actOnVectorColumn(
    elementRootForm, ringZero
  );
  MonomialUniversalEnvelopingOrdered<Coefficient> monomial;
  monomial.makeZero(ringZero, inputOwner);
  monomial.generatorsIndices.setSize(1);
  monomial.powers.setSize(1);
  monomial.powers.objects[0] = ringUnit;
  for (int index = 0; index < elementRootForm.size; index ++) {
    if (elementRootForm.objects[index] != 0) {
      monomial.coefficient = ringUnit;
      monomial.coefficient *= elementRootForm.objects[index];
      monomial.generatorsIndices[0] = index;
      this->addOnTop(monomial);
    }
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::raiseToPower(
  int power, const Coefficient& ringUnit
) {
  if (this->size == 0) {
    return;
  }
  ElementUniversalEnvelopingOrdered<Coefficient> buffer;
  buffer.operator=(*this);
  this->makeConstant(ringUnit, *this->owner);
  for (int i = 0; i < power; i ++) {
    this->operator*=(buffer);
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::lieBracketOnTheRight(
  const ElementSemisimpleLieAlgebra<Rational>& right,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered<Coefficient> element;
  element.assignElementLieAlgebra(right, ringUnit, ringZero, *this->owner);
  this->lieBracketOnTheRight(element, *this);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::lieBracketOnTheRight(
  const ElementUniversalEnvelopingOrdered<Coefficient>& right,
  ElementUniversalEnvelopingOrdered<Coefficient>& output
) {
  ElementUniversalEnvelopingOrdered<Coefficient> element;
  ElementUniversalEnvelopingOrdered<Coefficient> element2;
  element = *this;
  element *= right;
  element2 = right;
  element2 *= *this;
  output = element;
  output -= element2;
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::addMonomial(
  const MonomialUniversalEnvelopingOrdered<Coefficient>& input
) {
  if (input.isEqualToZero()) {
    return;
  }
  int index = this->getIndex(input);
  if (index == - 1) {
    this->addOnTop(input);
  } else {
    this->objects[index].coefficient += input.coefficient;
    if (this->objects[index].coefficient.isEqualToZero()) {
      this->removeIndexSwapWithLast(index);
    }
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::modOutVermaRelationsOld(
  bool substitutionHighestWeightWithZeroes,
  const PolynomialSubstitution<Rational>& highestWeightSub,
  const Coefficient& ringUnit
) {
  MonomialUniversalEnvelopingOrdered<Coefficient> monomial;
  ElementUniversalEnvelopingOrdered<Coefficient> output;
  output.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    monomial = this->objects[i];
    monomial.modOutVermaRelationsOld(
      substitutionHighestWeightWithZeroes, highestWeightSub, ringUnit
    );
    output.addMonomial(monomial);
  }
  this->operator=(output);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::modOutVermaRelations(
  const List<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  MonomialUniversalEnvelopingOrdered<Coefficient> monomial;
  ElementUniversalEnvelopingOrdered<Coefficient> output;
  output.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    monomial = this->objects[i];
    monomial.modOutVermaRelations(
      substitutionHiGoesToIthElement, ringUnit, ringZero
    );
    output.addMonomial(monomial);
  }
  this->operator=(output);
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::modOutVermaRelations(
  const List<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  int numberOfPositiveRoots =
  this->owner->ownerSemisimpleLieAlgebra->getNumberOfPositiveRoots();
  int dimension = this->owner->ownerSemisimpleLieAlgebra->getRank();
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int indexCurrentGenerator = this->generatorsIndices[i];
    if (indexCurrentGenerator >= numberOfPositiveRoots + dimension) {
      this->makeZero(ringZero, *this->owner);
      return;
    }
    if (indexCurrentGenerator < numberOfPositiveRoots) {
      return;
    }
    if (
      indexCurrentGenerator >= numberOfPositiveRoots &&
      indexCurrentGenerator < numberOfPositiveRoots + dimension
    ) {
      int degree = 0;
      if (!this->powers[i].isSmallInteger(degree)) {
        return;
      }
      if (substitutionHiGoesToIthElement == 0) {
        this->makeZero(ringZero, *this->owner);
        return;
      }
      Coefficient substitutedH = ringZero;
      Vector<Rational> currentH =
      this->owner->elementOrder[indexCurrentGenerator].getCartanPart();
      for (int j = 0; j < currentH.size; j ++) {
        substitutedH += (*substitutionHiGoesToIthElement)[j] * currentH[j];
      }
      MathRoutines::raiseToPower(substitutedH, degree, ringUnit);
      this->coefficient *= substitutedH;
      this->generatorsIndices.size --;
      this->powers.size --;
    }
  }
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::
splitCharacterOverReductiveSubalgebra(
  std::string* report,
  CharacterSemisimpleLieAlgebraModule& output,
  BranchingData& inputData
) {
  if (this->isEqualToZero()) {
    return false;
  }
  this->checkNonZeroOwner();
  WeylGroupData& weylGroup = this->getOwner()->weylGroup;
  std::stringstream out;
  std::string currentString;
  inputData.initializeAfterParabolicSelectionAndHomomorphism();
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms&
  weylGroupFiniteDimensionalSmallAsSubgroupInLarge =
  inputData.weylGroupFiniteDimensionalSmallAsSubgroupInLarge;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms&
  weylGroupFiniteDimensionalSmall =
  inputData.weylGroupFiniteDimensionalSmall;
  SemisimpleLieAlgebra& smallAlgebra = inputData.homomorphism.domainAlgebra();
  Vectors<Rational>& embeddingsSimpleEiGoesTo =
  inputData.homomorphism.imagesCartanDomain;
  CharacterSemisimpleLieAlgebraModule charAmbientFDWeyl;
  CharacterSemisimpleLieAlgebraModule remainingCharProjected;
  CharacterSemisimpleLieAlgebraModule remainingCharDominantLevI;
  Weight<Coefficient> workingMonomial;
  Weight<Coefficient> localHighest;
  List<Coefficient> multiplicities;
  HashedList<Vector<Coefficient> > tempHashedRoots;
  Coefficient bufferCoefficient;
  Coefficient highestCoefficient;
  for (int i = 0; i < this->size(); i ++) {
    const Weight<Coefficient>& monomial = (*this)[i];
    if (
      !inputData.weylGroupFiniteDimensional.
      freudenthalFormulaIrrepIsWRTLeviPart(
        monomial.weightFundamentalCoordinates,
        tempHashedRoots,
        multiplicities,
        currentString,
        10000
      )
    ) {
      if (report != nullptr) {
        *report = currentString;
      }
      return false;
    }
    for (int j = 0; j < tempHashedRoots.size; j ++) {
      bufferCoefficient = this->coefficients[i];
      workingMonomial.weightFundamentalCoordinates =
      weylGroup.getFundamentalCoordinatesFromSimple(tempHashedRoots[j]);
      workingMonomial.owner = this->getOwner();
      bufferCoefficient *= multiplicities[j];
      charAmbientFDWeyl.addMonomial(workingMonomial, bufferCoefficient);
    }
  }
  Vectors<Coefficient> orbitDominant;
  for (int i = 0; i < charAmbientFDWeyl.size(); i ++) {
    orbitDominant.setSize(0);
    if (
      !inputData.weylGroupFiniteDimensional.generateOrbitReturnFalseIfTruncated
      (
        weylGroup.getSimpleCoordinatesFromFundamental(
          charAmbientFDWeyl[i].weightFundamentalCoordinates
        ),
        orbitDominant,
        true,
        10000
      )
    ) {
      out
      << "Failed to generate the complement-sub-Weyl-orbit of weight "
      << weylGroup.getSimpleCoordinatesFromFundamental(
        charAmbientFDWeyl[i].weightFundamentalCoordinates
      ).toString();
      if (report != nullptr) {
        *report = out.str();
      }
      return false;
    }
    workingMonomial.owner = this->getOwner();
    for (int k = 0; k < orbitDominant.size; k ++) {
      if (
        weylGroupFiniteDimensionalSmallAsSubgroupInLarge.isDominantWeight(
          orbitDominant[k]
        )
      ) {
        workingMonomial.weightFundamentalCoordinates =
        weylGroup.getFundamentalCoordinatesFromSimple(orbitDominant[k]);
        remainingCharDominantLevI.addMonomial(
          workingMonomial, charAmbientFDWeyl.coefficients[i]
        );
      }
    }
  }
  FormatExpressions format;
  format.flagUseLatex = true;
  format.customPlusSign = "\\oplus ";
  format.fundamentalWeightLetter = "\\omega";
  out
  << "<br>Character w.r.t Levi part of the parabolic of the larger algebra: "
  << HtmlRoutines::getMathNoDisplay(
    remainingCharDominantLevI.toString(&format)
  );
  remainingCharProjected.makeZero();
  Vector<Coefficient> fundamentalCoordinatesSmaller;
  Vector<Coefficient> inSimpleCoordinates;
  fundamentalCoordinatesSmaller.setSize(
    weylGroupFiniteDimensionalSmall.ambientWeyl->getDimension()
  );
  for (int i = 0; i < remainingCharDominantLevI.size(); i ++) {
    inSimpleCoordinates =
    weylGroup.getSimpleCoordinatesFromFundamental(
      remainingCharDominantLevI[i].weightFundamentalCoordinates
    );
    for (
      int j = 0; j < weylGroupFiniteDimensionalSmall.ambientWeyl->
      getDimension(); j ++
    ) {
      fundamentalCoordinatesSmaller[j] =
      weylGroup.rootScalarCartanRoot(
        inSimpleCoordinates, embeddingsSimpleEiGoesTo[j]
      );
      fundamentalCoordinatesSmaller[j] /=
      weylGroupFiniteDimensionalSmall.ambientWeyl->cartanSymmetric(j, j) /
      2;
    }
    workingMonomial.owner = &smallAlgebra;
    workingMonomial.weightFundamentalCoordinates =
    fundamentalCoordinatesSmaller;
    remainingCharProjected.addMonomial(
      workingMonomial, remainingCharDominantLevI.coefficients[i]
    );
  }
  Vector<Coefficient> simpleGeneratorBaseField;
  output.makeZero();
  while (!remainingCharProjected.isEqualToZero()) {
    localHighest = *remainingCharProjected.monomials.lastObject();
    for (bool Found = true; Found;) {
      Found = false;
      for (
        int i = 0; i < weylGroupFiniteDimensionalSmall.rootsOfBorel.size; i
        ++
      ) {
        workingMonomial = localHighest;
        simpleGeneratorBaseField =
        weylGroupFiniteDimensionalSmall.rootsOfBorel[i];
        // <- implicit type conversion here!
        workingMonomial.weightFundamentalCoordinates +=
        weylGroupFiniteDimensionalSmall.ambientWeyl->
        getFundamentalCoordinatesFromSimple(simpleGeneratorBaseField);
        if (remainingCharProjected.monomials.contains(workingMonomial)) {
          localHighest = workingMonomial;
          Found = true;
        }
      }
    }
    highestCoefficient =
    remainingCharProjected.coefficients[
      remainingCharProjected.monomials.getIndex(localHighest)
    ];
    output.addMonomial(localHighest, highestCoefficient);
    if (
      !weylGroupFiniteDimensionalSmall.freudenthalFormulaIrrepIsWRTLeviPart(
        localHighest.weightFundamentalCoordinates,
        tempHashedRoots,
        multiplicities,
        currentString,
        10000
      )
    ) {
      if (report != nullptr) {
        *report = currentString;
      }
      return false;
    }
    for (int i = 0; i < tempHashedRoots.size; i ++) {
      workingMonomial.owner = &smallAlgebra;
      workingMonomial.weightFundamentalCoordinates =
      weylGroupFiniteDimensionalSmall.ambientWeyl->
      getFundamentalCoordinatesFromSimple(tempHashedRoots[i]);
      bufferCoefficient = multiplicities[i];
      bufferCoefficient *= highestCoefficient;
      remainingCharProjected.subtractMonomial(
        workingMonomial, bufferCoefficient
      );
    }
  }
  format.fundamentalWeightLetter = "\\psi";
  out
  << "<br>Character w.r.t the Levi part "
  << "of the parabolic of the small algebra: "
  << HtmlRoutines::getMathNoDisplay(output.toString(&format));
  if (report != nullptr) {
    DrawingVariables drawingVariables1;
    std::string currentString;
    output.drawMeNoMultiplicities(currentString, drawingVariables1, 10000);
    Vector<Rational> leftRoot;
    Vector<Rational> rightRoot;
    weylGroupFiniteDimensionalSmall.ambientWeyl->group.computeAllElements(
      false, 20
    );
    out << "<hr>";
    for (int i = 0; i < output.size(); i ++) {
      leftRoot =
      weylGroupFiniteDimensionalSmall.ambientWeyl->
      getSimpleCoordinatesFromFundamental(
        output[i].weightFundamentalCoordinates
      ).getVectorRational();
      std::stringstream currentStream;
      currentStream << output.coefficients[i].toString();
      drawingVariables1.drawTextAtVector(
        leftRoot, currentStream.str(), "black"
      );
      for (
        int j = 1; j < weylGroupFiniteDimensionalSmall.ambientWeyl->group.
        elements.size; j ++
      ) {
        rightRoot = leftRoot;
        weylGroupFiniteDimensionalSmall.ambientWeyl->actOnRhoModified(
          j, rightRoot
        );
        drawingVariables1.drawCircleAtVectorBufferRational(
          rightRoot, "#a00000", 5
        );
      }
    }
    out
    << "<hr>"
    << drawingVariables1.getHTMLDiv(
      weylGroupFiniteDimensionalSmall.ambientWeyl->getDimension(), true
    );
    *report = out.str();
  }
  return true;
}

#endif // header_math_extra_lie_theory_extras_ALREADY_INCLUDED
