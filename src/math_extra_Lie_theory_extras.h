// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderLieTheoryMiscellaneous
#define vpfHeaderLieTheoryMiscellaneous

#include "math_extra_semisimple_Lie_algebras.h"
#include "math_rational_function.h"
// #include "math_extra_universal_enveloping.h"

// the following data is isolated in a struct because it is
// way too large a lump to pass separately
struct BranchingData {
  HomomorphismSemisimpleLieAlgebra theHmm;
  FormatExpressions theFormat;
  Vector<RationalFunction<Rational> > theWeightFundCoords;
  CharacterSemisimpleLieAlgebraModule<RationalFunction<Rational> > theAmbientChar;
  CharacterSemisimpleLieAlgebraModule<RationalFunction<Rational> > theSmallCharFDpart;
  Selection selInducing;
  Selection selSmallParSel;
  Selection SelSplittingParSel;
  Vectors<Rational> weightsNilradicalLarge;
  Vectors<Rational> weightsNilradicalSmall;
  Vectors<Rational> weightsNilModPreNil;
  List<int> indicesNilradicalSmall;
  List<int> indicesNilradicalLarge;
  List<ElementSemisimpleLieAlgebra<Rational> > nilradicalLarge;
  List<ElementSemisimpleLieAlgebra<Rational> > nilradicalSmall;
  List<ElementSemisimpleLieAlgebra<Rational> > NilModPreNil;
  Vectors<RationalFunction<Rational> > outputWeightsFundCoordS;
  Vectors<RationalFunction<Rational> > outputWeightsSimpleCoords;
  Vectors<RationalFunction<Rational> > g2Weights;
  Vectors<RationalFunction<Rational> > g2DualWeights;
  Vectors<RationalFunction<Rational> > leviEigenSpace;
  Vectors<Rational> generatorsSmallSub;
  HashedList<RationalFunction<Rational> > theCharacterDifferences;
  List<ElementUniversalEnveloping<RationalFunction<Rational> > > outputEigenWords;
  List<RationalFunction<Rational> > theChars;
  List<ElementSumGeneralizedVermas<RationalFunction<Rational> > > theEigenVectorS;
  List<ElementUniversalEnveloping<RationalFunction<Rational> > > theUEelts;
  List<Rational> additionalMultipliers;
  List<RationalFunction<Rational> > theShapovalovProducts;
  List<ElementSumGeneralizedVermas<RationalFunction<Rational> > > theEigenVectorsLevi;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms WeylFD;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms WeylFDSmallAsSubInLarge;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms WeylFDSmall;
  bool flagUseNilWeightGeneratorOrder;
  bool flagAscendingGeneratorOrder;
  std::string getStringCasimirProjector(int theIndex, const Rational& additionalMultiple);
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
  SemisimpleLieAlgebra* theOwner;
  // the format of the order is arbitrary except for the following requirements:
  // -All elements of the order must be either 1) nilpotent or 2) elements of the Cartan
  // -Let the number of positive roots be N and the rank be K. Then the indices N,..., N+K- 1 must
  //  correspond to the elements of the Cartan.
  List<ElementSemisimpleLieAlgebra<Rational> > theOrder;
  // The order of chevalley generators is as follows. First come negative roots,
  // then elements of cartan, then positive Vectors<Rational>
  // The weights are in increasing order
  // The i^th column of the matrix gives the coordinates of the i^th Chevalley generator
  // in the current coordinates
  Matrix<Rational> ChevalleyGeneratorsInCurrentCoords;
  void assignGeneratorCoefficientOne(int theIndex, ElementSemisimpleLieAlgebra<Rational>& output) {
    output.operator=(this->theOrder[theIndex]);
  }
  SemisimpleLieAlgebraOrdered();
  bool checkInitialization() const;
  int getDisplayIndexFromGeneratorIndex(int GeneratorIndex);
  void getLinearCombinationFrom(ElementSemisimpleLieAlgebra<Rational>& input, Vector<Rational>& theCoeffs);
  void initialize(List<ElementSemisimpleLieAlgebra<Rational> >& inputOrder, SemisimpleLieAlgebra& owner);
  void initDefaultOrder(SemisimpleLieAlgebra& owner);
};

template <class Coefficient>
Vector<Coefficient> BranchingData::projectWeight(Vector<Coefficient>& input) {
  Vector<Coefficient> result;
  Vector<Coefficient> fundCoordsSmaller;
  fundCoordsSmaller.makeZero(this->theHmm.theDomain().getRank());
  for (int j = 0; j < this->theHmm.theDomain().getRank(); j ++) {
    fundCoordsSmaller[j] = this->theHmm.theRange().theWeyl.RootScalarCartanRoot(input, theHmm.ImagesCartanDomain[j]);
    fundCoordsSmaller[j] /= this->theHmm.theDomain().theWeyl.cartanSymmetric.elements[j][j] / 2;
  }
  result = this->theHmm.theDomain().theWeyl.getSimpleCoordinatesFromFundamental(fundCoordsSmaller);
  return result;
}

class GeneralizedVermaModuleCharacters {
public:
  PauseThread thePauseControlleR;
  List<Matrix<Rational> > theLinearOperators;
  // the first k variables correspond to the Cartan of the smaller Lie algebra
  // the next l variables correspond to the Cartan of the larger Lie algebra
  // the last variable is the projectivization
  List<Matrix<Rational> > theLinearOperatorsExtended;
  Vector<Rational>  NonIntegralOriginModificationBasisChanged;
  std::fstream theMultiplicitiesMaxOutput;
  std::fstream theMultiplicitiesMaxOutputReport2;
  Vectors<Rational> GmodKnegativeWeightS;
  Vectors<Rational> GmodKNegWeightsBasisChanged;
  Matrix<Rational> preferredBasisChangE;
  Matrix<Rational> preferredBasisChangeInversE;
  Vectors<Rational> preferredBasiS;
  Cone PreimageWeylChamberLargerAlgebra;
  Cone WeylChamberSmallerAlgebra;
  Cone PreimageWeylChamberSmallerAlgebra;
  Lattice theExtendedIntegralLatticeMatForM;
  List<QuasiPolynomial> theQPsNonSubstituted;
  List<List<QuasiPolynomial> > theQPsSubstituted;
  List<QuasiPolynomial> theMultiplicities;
  HomomorphismSemisimpleLieAlgebra theHmm;
  // List<QuasiPolynomial> theMultiplicitiesExtremaCandidates;
  int UpperLimitChambersForDebugPurposes;
  int numNonZeroMults;
  Selection ParabolicLeviPartRootSpacesZeroStandsForSelected;
  Selection ParabolicSelectionSmallerAlgebra;
  List<Rational> theCoeffs;
  Vectors<Rational> theTranslationS;
  Vectors<Rational> theTranslationsProjectedBasisChanged;
  PartFractions thePfs;
  // List<Cone> allParamSubChambersRepetitionsAllowedConeForm;
  ConeComplex projectivizedParamComplex;
  ConeLatticeAndShiftMaxComputation theMaxComputation;
  ConeComplex smallerAlgebraChamber;
  ConeComplex projectivizedChambeR;
  std::stringstream log;
  WeylGroupData* WeylSmaller;
  WeylGroupData* WeylLarger;
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
  std::string prepareReportOneCone(FormatExpressions& theFormat, const Cone& theCone);
  void getProjection(int indexOperator, const Vector<Rational>& input, Vector<Rational> & output);
  void splitByMultiplicityFreeWall(Cone& theCone, ConeComplex& output);
  void initTheMaxComputation();
  void computeQPsFromChamberComplex();
  void getSubstitutionFromIndex(
    PolynomialSubstitution<Rational>& outputSub,
    Matrix<LargeInteger>& outputMat,
    LargeIntegerUnsigned& ouputDen,
    int theIndex
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
  void SimplifyAccumulateInOutputNoOutputInit(
    ElementUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  MonomialUniversalEnvelopingOrdered(const MonomialUniversalEnvelopingOrdered& other);
public:
  SemisimpleLieAlgebraOrdered* owner;
  std::string DebugString;
  std::string toString(FormatExpressions* PolyFormatLocal) const;
  // SelectedIndices gives the non-zero powers of the generators participating in the monomial
  // powers gives the powers of the generators in the order specified in the owner
  List<int> generatorsIndices;
  List<Coefficient> powers;
  Coefficient theCoefficient;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void multiplyBy(
    const MonomialUniversalEnveloping<Coefficient>& other,
    ElementUniversalEnvelopingOrdered<Coefficient>& output
  );
  void multiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const Coefficient& thePower);
  void multiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower);
  void multiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other);
  void makeZero(int numVars, SemisimpleLieAlgebraOrdered& theOwner);
  void makeZero(const Coefficient& ringZero, SemisimpleLieAlgebraOrdered& theOwner);
  bool modOutFDRelationsExperimental(
    const Vector<Rational>& theHWsimpleCoords,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void modOutVermaRelations(
    const List<Coefficient>* subHiGoesToIthElement = 0,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void setNumberOfVariables(int newNumVars);
  unsigned int hashFunction() const;
  static inline unsigned int hashFunction(const MonomialUniversalEnvelopingOrdered<Coefficient>& input) {
    return input.hashFunction();
  }
  void getDegree(Polynomial<Rational>& output) {
    output.makeZero(this->Coefficient.NumVars);
    for (int i = 0; i < this->generatorsIndices.size; i ++) {
      output += this->powers[i];
    }
  }
  bool getElementUniversalEnveloping(ElementUniversalEnveloping<Coefficient>& output, SemisimpleLieAlgebraOrdered& owner);
  bool isEqualToZero() const {
    return this->Coefficient.isEqualToZero();
  }
  bool commutingLeftIndexAroundRightIndexAllowed(
    Coefficient& theLeftPower,
    int leftGeneratorIndex,
    Coefficient& theRightPower,
    int rightGeneratorIndex
  );
  bool commutingRightIndexAroundLeftIndexAllowed(
    Coefficient& theLeftPower, int leftGeneratorIndex, Coefficient& theRightPower, int rightGeneratorIndex
  );
  bool switchConsecutiveIndicesIfTheyCommute(
    int theLeftIndex,
    MonomialUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringZero = 0
  );
  void makeConstant(const Coefficient& theConst, SemisimpleLieAlgebraOrdered& theOwner) {
    this->generatorsIndices.size = 0;
    this->powers.size = 0;
    this->Coefficient = theConst;
    this->owner = &theOwner;
  }
  void simplify(
    ElementUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void commuteConsecutiveIndicesLeftIndexAroundRight(
    int theIndeX,
    ElementUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void commuteConsecutiveIndicesRightIndexAroundLeft(
    int theIndeX,
    ElementUniversalEnvelopingOrdered<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  MonomialUniversalEnvelopingOrdered() {
    this->owner = nullptr;
  }
  void substitutionCoefficients(PolynomialSubstitution<Rational>& theSub);
  bool operator==(const MonomialUniversalEnvelopingOrdered& other) const {
    if (this->owner != other.owner) {
      global.fatal << "Attempt to compare universal enveloping algebra monomials with different owners. " << global.fatal;
    }
    return this->powers == other.powers && this->generatorsIndices == other.generatorsIndices;
  }
  void operator*=(const MonomialUniversalEnvelopingOrdered& other);
  template<class OtherCoefficientType>
  void assignChangeCoefficientType(const MonomialUniversalEnvelopingOrdered<OtherCoefficientType>& other) {
    this->Coefficient = other.Coefficient;
    this->generatorsIndices = other.generatorsIndices.size;
    this->powers.setSize(other.powers.size);
    for (int i = 0; i < this->powers.size; i ++) {
      this->powers[i] = other.powers[i];
    }
  }
  inline void operator=(const MonomialUniversalEnvelopingOrdered& other) {
    this->generatorsIndices = other.generatorsIndices;
    this->powers = other.powers;
    this->Coefficient = other.Coefficient;
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
  std::string toString(FormatExpressions* PolyFormatLocal) const {
    std::string tempS;
    this->toString(tempS, PolyFormatLocal);
    return tempS;
  }
  bool needsParenthesisForMultiplication() const {
    return this->size > 1;
  }
  SemisimpleLieAlgebraOrdered* owner;
  void addMonomial(const MonomialUniversalEnvelopingOrdered<Coefficient>& input);
  void makeCartanGenerator(const Vector<Rational> & input, int numVars, SemisimpleLieAlgebraOrdered& theOwner);
  void assignElementLieAlgebra(
    const ElementSemisimpleLieAlgebra<Rational>& input,
    const Coefficient& ringUnit,
    const Coefficient& ringZero,
    SemisimpleLieAlgebraOrdered& theOwner
  );
  void makeZero(SemisimpleLieAlgebraOrdered& theOwner);
  bool AssignElementUniversalEnveloping(
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
    const Vector<Rational> & theHWsimpleCoords,
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
  void substitutionCoefficients(PolynomialSubstitution<Rational>& theSub);
  void makeConstant(const Coefficient& coeff, SemisimpleLieAlgebraOrdered& theOwner) {
    this->makeZero(theOwner);
    MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
    tempMon.makeConstant(coeff, theOwner);
    this->addMonomial(tempMon);
  }
  void simplify(const Coefficient& ringUnit = 1,  const Coefficient& ringZero = 0);
  int minimalNumberOfVariables() const {
    if (this->size == 0) {
      return 0;
    } else {
      return this->objects[0].Coefficient.NumVars;
    }
  }
  inline void multiplyBy(const ElementUniversalEnvelopingOrdered& other) {
    this->operator*=(other);
  }
  void modOutVermaRelations(
    const List<Coefficient>* subHiGoesToIthElement = 0,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void modOutVermaRelationsOld(
    bool SubHighestWeightWithZeroes,
    const PolynomialSubstitution<Rational>& highestWeightSub,
    const Coefficient& ringUnit
  );
  template<class CoefficientTypeQuotientField>
  static void getBasisFromSpanOfElements(
    List<ElementUniversalEnvelopingOrdered<Coefficient> >& theElements,
    Vectors<CoefficientTypeQuotientField>& outputCoords,
    List<ElementUniversalEnvelopingOrdered<Coefficient> >& outputTheBasis,
    const CoefficientTypeQuotientField& theFieldUnit,
    const CoefficientTypeQuotientField& theFieldZero
  );
  static void getBasisFromSpanOfElements(
    List<ElementUniversalEnvelopingOrdered>& theElements,
    Vectors<Polynomial<Coefficient> >& outputCoordinates,
    List<ElementUniversalEnvelopingOrdered>& outputTheBasis
  );
  bool getCoordinatesInBasis(
    List<ElementUniversalEnvelopingOrdered<Coefficient> >& theBasis,
    Vector<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  ) const;
  static void getCoordinateFormOfSpanOfElements(
    List<ElementUniversalEnvelopingOrdered>& theElements,
    Vectors<Polynomial<Coefficient> >& outputCoordinates,
    ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials
  );
  void assignFromCoordinateFormWithRespectToBasis(
    List<ElementUniversalEnveloping<Coefficient> >& theBasis,
    Vector<Polynomial<Coefficient> >& input,
    SemisimpleLieAlgebraOrdered& owner
  );
  void raiseToPower(int thePower, const Coefficient& ringUnit);
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
  void makeCasimir(SemisimpleLieAlgebraOrdered& theOwner, int numVars);
  void actOnMe(const ElementSemisimpleLieAlgebra<Rational>& theElt, ElementUniversalEnvelopingOrdered& output);
  void lieBracketOnTheRight(const ElementUniversalEnvelopingOrdered& right, ElementUniversalEnvelopingOrdered& output);
  void lieBracketOnTheRight(
    const ElementSemisimpleLieAlgebra<Rational>& right, const Coefficient& ringUnit, const Coefficient& ringZero
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
          this->objects[j].theCoefficient *= currentCoeff;
        }
      }
      outputWasMultipliedBy *= currentCoeff;
    }
  }
};

template <class Coefficient>
class ElementVermaModuleOrdered {
public:
  ElementUniversalEnvelopingOrdered<Coefficient> theElT;
  std::string DebugString;
  PolynomialSubstitution<Coefficient> theSubNthElementIsImageNthCoordSimpleBasis;
  void ComputeDebugString() {
    FormatExpressions tempFormat;
    this->DebugString = this->toString(tempFormat);
  }
  std::string toString(const FormatExpressions& theFormat) const;
  bool isEqualToZero() const {
    return this->theElT.isEqualToZero();
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
    List<ElementVermaModuleOrdered<Coefficient> >& theElements,
    Vectors<RationalFunction<Rational> >& outputCoordinates,
    List<ElementVermaModuleOrdered>& outputTheBasis,
    const RationalFunction<Rational>& RFOne,
    const RationalFunction<Rational>& RFZero
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
    return this->theElT.isProportionalTo(other.theElT, outputTimesMeEqualsOther, ringZero);
  }
  void makeZero(SemisimpleLieAlgebraOrdered& owner, PolynomialSubstitution<Rational>& incomingSub) {
    this->theElT.makeZero(owner);
    this->theSubNthElementIsImageNthCoordSimpleBasis = incomingSub;
  }
  template <class CoefficientTypeOther>
  void operator*=(const CoefficientTypeOther& theConst) {
    this->theElT.operator*=(theConst);
  }
  void multiplyOnTheLeft(
    const ElementSemisimpleLieAlgebra<Rational>& other,
    ElementVermaModuleOrdered<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  void clearDenominators(Coefficient& outputWasMultipliedBy, const Coefficient& ringUnit) {
    this->theElT.clearDenominators(outputWasMultipliedBy, ringUnit);
  }
  void operator/=(const Coefficient& theConst) {
    this->theElT.operator/=(theConst);
  }
  void operator-=(const ElementVermaModuleOrdered& other) {
    this->theElT -= other.theElT;
  }
  void operator+=(const ElementVermaModuleOrdered& other) {
    this->theElT += other.theElT;
  }
  void operator=(const ElementVermaModuleOrdered& other) {
    this->theElT = other.theElT;
    this->theSubNthElementIsImageNthCoordSimpleBasis = other.theSubNthElementIsImageNthCoordSimpleBasis;
  }
};
#endif
