// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderLieTheoryMiscellaneous
#define vpfHeaderLieTheoryMiscellaneous

#include "math_extra_semisimple_lie_algebras.h"
#include "math_rational_function.h"
#include "math_extra_modules_semisimple_lie_algebras.h"
// #include "math_extra_universal_enveloping.h"

// the following data is isolated in a struct because it is
// way too large a lump to pass separately
struct BranchingData {
  HomomorphismSemisimpleLieAlgebra homomorphism;
  FormatExpressions format;
  Vector<RationalFraction<Rational> > weightFundamentalCoordinates;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > ambientCharacter;
  CharacterSemisimpleLieAlgebraModule<RationalFraction<Rational> > smallCharacterFiniteDimensionalPart;
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
  List<ElementSemisimpleLieAlgebra<Rational> > nilradicalModuloPreimageNilradical;
  Vectors<RationalFraction<Rational> > outputWeightsFundCoordS;
  Vectors<RationalFraction<Rational> > outputWeightsSimpleCoords;
  Vectors<RationalFraction<Rational> > g2Weights;
  Vectors<RationalFraction<Rational> > g2DualWeights;
  Vectors<RationalFraction<Rational> > leviEigenSpace;
  Vectors<Rational> generatorsSmallSub;
  HashedList<RationalFraction<Rational> > characterDifferences;
  List<ElementUniversalEnveloping<RationalFraction<Rational> > > outputEigenWords;
  List<RationalFraction<Rational> > allCharacters;
  List<ElementSumGeneralizedVermas<RationalFraction<Rational> > > eigenVectors;
  List<ElementUniversalEnveloping<RationalFraction<Rational> > > elementsUniversalEnveloping;
  List<Rational> additionalMultipliers;
  List<RationalFraction<Rational> > shapovalovProducts;
  List<ElementSumGeneralizedVermas<RationalFraction<Rational> > > eigenVectorsLevi;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms weylGroupFiniteDimensional;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms weylGroupFiniteDimensionalSmallAsSubgroupInLarge;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms weylGroupFiniteDimensionalSmall;
  bool flagUseNilWeightGeneratorOrder;
  bool flagAscendingGeneratorOrder;
  std::string getStringCasimirProjector(int index, const Rational& additionalMultiple);
  template <class Coefficient>
  Vector<Coefficient> projectWeight(Vector<Coefficient>& input);
  void resetOutputData();
  void initAssumingParSelAndHmmInittedPart1NoSubgroups();
  void initAssumingParSelAndHmmInittedPart2Subgroups();
  void initAssumingParSelAndHmmInitted() {
    MacroRegisterFunctionWithName("BranchingData::initAssumingParSelAndHmmInitted");
    this->initAssumingParSelAndHmmInittedPart1NoSubgroups();
    this->initAssumingParSelAndHmmInittedPart2Subgroups();
  }
  BranchingData();
};

class SemisimpleLieAlgebraOrdered {
public:
  SemisimpleLieAlgebra* ownerSemisimpleLieAlgebra;
  // the format of the order is arbitrary except for the following requirements:
  // -All elements of the order must be either 1) nilpotent or 2) elements of the Cartan
  // -Let the number of positive roots be N and the rank be K. Then the indices N,..., N+K- 1 must
  //  correspond to the elements of the Cartan.
  List<ElementSemisimpleLieAlgebra<Rational> > elementOrder;
  // The order of chevalley generators is as follows. First come negative roots,
  // then elements of cartan, then positive Vectors<Rational>
  // The weights are in increasing order
  // The i^th column of the matrix gives the coordinates of the i^th Chevalley generator
  // in the current coordinates
  Matrix<Rational> chevalleyGeneratorsInCurrentCoordinates;
  void assignGeneratorCoefficientOne(int index, ElementSemisimpleLieAlgebra<Rational>& output) {
    output.operator=(this->elementOrder[index]);
  }
  SemisimpleLieAlgebraOrdered();
  bool checkInitialization() const;
  int getDisplayIndexFromGeneratorIndex(int generatorIndex);
  void getLinearCombinationFrom(ElementSemisimpleLieAlgebra<Rational>& input, Vector<Rational>& coefficients);
  void initialize(List<ElementSemisimpleLieAlgebra<Rational> >& inputOrder, SemisimpleLieAlgebra& owner);
  void initDefaultOrder(SemisimpleLieAlgebra& owner);
};

template <class Coefficient>
Vector<Coefficient> BranchingData::projectWeight(Vector<Coefficient>& input) {
  Vector<Coefficient> result;
  Vector<Coefficient> fundamentalCoordinatesSmaller;
  fundamentalCoordinatesSmaller.makeZero(this->homomorphism.domainAlgebra().getRank());
  for (int j = 0; j < this->homomorphism.domainAlgebra().getRank(); j ++) {
    fundamentalCoordinatesSmaller[j] = this->homomorphism.coDomainAlgebra().weylGroup.rootScalarCartanRoot(input, homomorphism.imagesCartanDomain[j]);
    fundamentalCoordinatesSmaller[j] /= this->homomorphism.domainAlgebra().weylGroup.cartanSymmetric.elements[j][j] / 2;
  }
  result = this->homomorphism.domainAlgebra().weylGroup.getSimpleCoordinatesFromFundamental(
    fundamentalCoordinatesSmaller, Coefficient::zero()
  );
  return result;
}

class GeneralizedVermaModuleCharacters {
public:
  PauseThread pauseController;
  List<Matrix<Rational> > linearOperators;
  // the first k variables correspond to the Cartan of the smaller Lie algebra
  // the next l variables correspond to the Cartan of the larger Lie algebra
  // the last variable is the projectivization
  List<Matrix<Rational> > linearOperatorsExtended;
  Vector<Rational> NonIntegralOriginModificationBasisChanged;
  std::fstream multiplicitiesMaxOutputReport2;
  Vectors<Rational> gModKNegativeWeights;
  Vectors<Rational> gModKNegativeWeightsBasisChanged;
  Matrix<Rational> preferredBasisChangE;
  Matrix<Rational> preferredBasisChangeInversE;
  Vectors<Rational> preferredBasis;
  Cone PreimageWeylChamberLargerAlgebra;
  Cone WeylChamberSmallerAlgebra;
  Cone PreimageWeylChamberSmallerAlgebra;
  Lattice extendedIntegralLatticeMatrixForm;
  List<QuasiPolynomial> theQPsNonSubstituted;
  List<List<QuasiPolynomial> > theQPsSubstituted;
  List<QuasiPolynomial> multiplicities;
  HomomorphismSemisimpleLieAlgebra homomorphism;
  // List<QuasiPolynomial> theMultiplicitiesExtremaCandidates;
  int UpperLimitChambersForDebugPurposes;
  int numberNonZeroMultiplicities;
  Selection ParabolicLeviPartRootSpacesZeroStandsForSelected;
  Selection ParabolicSelectionSmallerAlgebra;
  List<Rational> coefficients;
  Vectors<Rational> translations;
  Vectors<Rational> translationsProjectedBasisChanged;
  PartialFractions partialFractions;
  // List<Cone> allParamSubChambersRepetitionsAllowedConeForm;
  ConeComplex projectivizedParamComplex;
  ConeLatticeAndShiftMaxComputation maximumComputation;
  ConeComplex smallerAlgebraChamber;
  ConeComplex projectivizedChambeR;
  std::stringstream log;
  WeylGroupData* weylSmaller;
  WeylGroupData* weylLarger;
  int computationPhase;
  int NumProcessedConesParam;
  int NumProcessedExtremaEqualOne;
  std::string computeMultiplicitiesLargerAlgebraHighestWeight(
    Vector<Rational>& highestWeightLargerAlgebraFundamentalCoords, Vector<Rational>& parabolicSel
  );
  std::string checkMultiplicitiesVsOrbits();
  std::string elementToStringMultiplicitiesReport();
  void incrementComputation(Vector<Rational>& parabolicSel);
  std::string prepareReport();
  GeneralizedVermaModuleCharacters();
  bool checkInitialization() const;
  std::string prepareReportOneCone(FormatExpressions& format, const Cone& cone);
  void getProjection(int indexOperator, const Vector<Rational>& input, Vector<Rational>& output);
  void splitByMultiplicityFreeWall(Cone& cone, ConeComplex& output);
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
    Matrix<Rational>& output, Vector<Rational>& outputTranslation, Vectors<Rational>& NonParams, int numParams
  );
  void initFromHomomorphism(Vector<Rational>& theParabolicSel, HomomorphismSemisimpleLieAlgebra& input);
  void transformToWeylProjectiveStep1();
  void transformToWeylProjectiveStep2();
  void transformToWeylProjective(int indexOperator, Vector<Rational>& startingNormal, Vector<Rational>& outputNormal);
};

template<class Coefficient>
class ElementUniversalEnvelopingOrdered;

template<class Coefficient>
class MonomialUniversalEnvelopingOrdered {
  void simplifyAccumulateInOutputNoOutputInit(
    ElementUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  MonomialUniversalEnvelopingOrdered(const MonomialUniversalEnvelopingOrdered& other);
public:
  SemisimpleLieAlgebraOrdered* owner;
  // std::string DebugString;
  std::string toString(FormatExpressions* format) const;
  // SelectedIndices gives the non-zero powers of the generators participating in the monomial
  // powers gives the powers of the generators in the order specified in the owner
  List<int> generatorsIndices;
  List<Coefficient> powers;
  Coefficient coefficient;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void multiplyBy(
    const MonomialUniversalEnveloping<Coefficient>& other,
    ElementUniversalEnvelopingOrdered<Coefficient>& output
  );
  void multiplyByGeneratorPowerOnTheRight(int generatorIndex, const Coefficient& power);
  void multiplyByGeneratorPowerOnTheRight(int generatorIndex, int power);
  void multiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other);
  void makeZero(int numberOfVariables, SemisimpleLieAlgebraOrdered& inputOwner);
  void makeZero(const Coefficient& ringZero, SemisimpleLieAlgebraOrdered& inputOwner);
  bool modOutFDRelationsExperimental(
    const Vector<Rational>& theHWsimpleCoords,
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
  static inline unsigned int hashFunction(const MonomialUniversalEnvelopingOrdered<Coefficient>& input) {
    return input.hashFunction();
  }
  bool getElementUniversalEnveloping(ElementUniversalEnveloping<Coefficient>& output, SemisimpleLieAlgebraOrdered& owner);
  bool isEqualToZero() const {
    return this->Coefficient.isEqualToZero();
  }
  bool commutingLeftIndexAroundRightIndexAllowed(
    Coefficient& leftPower,
    int leftGeneratorIndex,
    Coefficient& rightPower,
    int rightGeneratorIndex
  );
  bool commutingRightIndexAroundLeftIndexAllowed(
    Coefficient& leftPower, int leftGeneratorIndex, Coefficient& rightPower, int rightGeneratorIndex
  );
  bool switchConsecutiveIndicesIfTheyCommute(
    int leftIndex,
    MonomialUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringZero = 0
  );
  void makeConstant(const Coefficient& inputConstant, SemisimpleLieAlgebraOrdered& inputOwner) {
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
  void substitutionCoefficients(PolynomialSubstitution<Rational>& substitution);
  bool operator==(const MonomialUniversalEnvelopingOrdered& other) const {
    if (this->owner != other.owner) {
      global.fatal << "Attempt to compare universal enveloping algebra monomials with different owners. " << global.fatal;
    }
    return this->powers == other.powers && this->generatorsIndices == other.generatorsIndices;
  }
  void operator*=(const MonomialUniversalEnvelopingOrdered& other);
  template<class OtherCoefficientType>
  void assignChangeCoefficientType(const MonomialUniversalEnvelopingOrdered<OtherCoefficientType>& other) {
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
bool MonomialUniversalEnvelopingOrdered<Coefficient>::flagAnErrorHasOccurredTimeToPanic = false;

template <class Coefficient>
class ElementUniversalEnvelopingOrdered : public HashedList<MonomialUniversalEnvelopingOrdered<Coefficient> > {
private:
  void addMonomialNoCleanUpZeroCoefficient(const MonomialUniversalEnvelopingOrdered<Coefficient>& input);
  void cleanUpZeroCoefficient();
  friend class MonomialUniversalEnvelopingOrdered<Coefficient>;
public:
  void toString(std::string& output, FormatExpressions* PolyFormatLocal) const;
  std::string toString(FormatExpressions* format) const {
    std::string tempS;
    this->toString(tempS, format);
    return tempS;
  }
  bool needsParenthesisForMultiplication() const {
    return this->size > 1;
  }
  SemisimpleLieAlgebraOrdered* owner;
  void addMonomial(const MonomialUniversalEnvelopingOrdered<Coefficient>& input);
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
    const Coefficient& inputCoeff,
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
    ElementUniversalEnveloping<Coefficient>& output, SemisimpleLieAlgebra& inputOwner
  );
  bool getLieAlgebraElementIfPossible(ElementSemisimpleLieAlgebra<Rational>& output) const;
  void substitutionCoefficients(PolynomialSubstitution<Rational>& substitution);
  void makeConstant(const Coefficient& coeff, SemisimpleLieAlgebraOrdered& inputOwner) {
    this->makeZero(inputOwner);
    MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
    tempMon.makeConstant(coeff, inputOwner);
    this->addMonomial(tempMon);
  }
  void simplify(const Coefficient& ringUnit = 1,  const Coefficient& ringZero = 0);
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
  template<class CoefficientTypeQuotientField>
  static void getBasisFromSpanOfElements(
    List<ElementUniversalEnvelopingOrdered<Coefficient> >& elements,
    Vectors<CoefficientTypeQuotientField>& outputCoords,
    List<ElementUniversalEnvelopingOrdered<Coefficient> >& outputTheBasis,
    const CoefficientTypeQuotientField& fieldUnit,
    const CoefficientTypeQuotientField& fieldZero
  );
  bool getCoordinatesInBasis(
    List<ElementUniversalEnvelopingOrdered<Coefficient> >& theBasis,
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
    MonomialUniversalEnvelopingOrdered<Coefficient>& tempMon = this->objects[0];
    if (!tempMon.Coefficient.isEqualToOne()) {
      return false;
    }
    if (tempMon.generatorsIndices.size != 1) {
      return false;
    }
    return true;
  }
  void actOnMe(const ElementSemisimpleLieAlgebra<Rational>& element, ElementUniversalEnvelopingOrdered& output);
  void lieBracketOnTheRight(const ElementUniversalEnvelopingOrdered& right, ElementUniversalEnvelopingOrdered& output);
  void lieBracketOnTheRight(
    const ElementSemisimpleLieAlgebra<Rational>& right,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  void operator=(const ElementUniversalEnvelopingOrdered& other) {
    this->::HashedList<MonomialUniversalEnvelopingOrdered<Coefficient> >::operator=(other);
    this->owner = other.owner;
  }
  template<class OtherCoefficientType>
  void assignChangeCoefficientType (const ElementUniversalEnvelopingOrdered<OtherCoefficientType>& other) {
    this->makeZero(*other.owner);
    MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
    this->reserve(other.size);
    for (int i = 0; i < other.size; i ++) {
      tempMon.assignChangeCoefficientType(other[i]);
      this->addMonomial(tempMon);
    }
  }
  void operator+=(const ElementUniversalEnvelopingOrdered& other);
  void operator+=(int other);
  void operator+=(const Rational& other);
  void operator-=(const ElementUniversalEnvelopingOrdered& other);
  void operator*=(const ElementUniversalEnvelopingOrdered& other);
  template <class SecondType>
  void operator/=(const SecondType& other);
  template<class otherType>
  void operator*=(const otherType& other);
  ElementUniversalEnvelopingOrdered() {
    this->owner = nullptr;
  }
  bool isProportionalTo(
    const ElementUniversalEnvelopingOrdered<Coefficient>& other,
    Coefficient& outputTimesMeEqualsOther,
    const Coefficient& ringZero
  ) const;
  ElementUniversalEnvelopingOrdered(const ElementUniversalEnvelopingOrdered& other) {
    this->operator=(other);
  }
  void clearDenominators(Coefficient& outputWasMultipliedBy, const Coefficient& ringUnit) {
    outputWasMultipliedBy = ringUnit;
    Coefficient currentCoeff;
    for (int i = 0; i < this->size; i ++) {
      MonomialUniversalEnvelopingOrdered<Coefficient>& currentMon = this->objects[i];
      currentMon.Coefficient.clearDenominators(currentCoeff);
      for (int j = 0; j < this->size; j ++) {
        if (j != i) {
          this->objects[j].coefficient *= currentCoeff;
        }
      }
      outputWasMultipliedBy *= currentCoeff;
    }
  }
};

template <class Coefficient>
class ElementVermaModuleOrdered {
public:
  ElementUniversalEnvelopingOrdered<Coefficient> elementInternal;
  PolynomialSubstitution<Coefficient> substitutionNthElementIsImageNthCoordinateSimpleBasis;
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
    const ElementSemisimpleLieAlgebra<Rational>& actingElt,
    ElementVermaModuleOrdered<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  ) const;
  static void getBasisFromSpanOfElements(
    List<ElementVermaModuleOrdered<Coefficient> >& elements,
    Vectors<RationalFraction<Rational> >& outputCoordinates,
    List<ElementVermaModuleOrdered>& outputTheBasis,
    const RationalFraction<Rational>& rationalFractionOne,
    const RationalFraction<Rational>& rationalFractionZero
  );
  bool getCoordinatesInBasis(
    const List<ElementVermaModuleOrdered<Coefficient> >& theBasis,
    Vector<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  ) const;
  bool isProportionalTo(
    const ElementVermaModuleOrdered<Coefficient>& other,
    Coefficient& outputTimesMeEqualsOther,
    const Coefficient& ringZero
  ) const {
    return this->elementInternal.isProportionalTo(other.elementInternal, outputTimesMeEqualsOther, ringZero);
  }
  void makeZero(SemisimpleLieAlgebraOrdered& owner, PolynomialSubstitution<Rational>& incomingSub) {
    this->elementInternal.makeZero(owner);
    this->substitutionNthElementIsImageNthCoordinateSimpleBasis = incomingSub;
  }
  template <class CoefficientTypeOther>
  void operator*=(const CoefficientTypeOther& theConst) {
    this->elementInternal.operator*=(theConst);
  }
  void multiplyOnTheLeft(
    const ElementSemisimpleLieAlgebra<Rational>& other,
    ElementVermaModuleOrdered<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  void clearDenominators(Coefficient& outputWasMultipliedBy, const Coefficient& ringUnit) {
    this->elementInternal.clearDenominators(outputWasMultipliedBy, ringUnit);
  }
  void operator/=(const Coefficient& theConst) {
    this->elementInternal.operator/=(theConst);
  }
  void operator-=(const ElementVermaModuleOrdered& other) {
    this->elementInternal -= other.elementInternal;
  }
  void operator+=(const ElementVermaModuleOrdered& other) {
    this->elementInternal += other.elementInternal;
  }
  void operator=(const ElementVermaModuleOrdered& other) {
    this->elementInternal = other.elementInternal;
    this->substitutionNthElementIsImageNthCoordinateSimpleBasis = other.substitutionNthElementIsImageNthCoordinateSimpleBasis;
  }
};
#endif
