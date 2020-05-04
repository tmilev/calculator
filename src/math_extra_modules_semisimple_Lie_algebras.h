// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader_HeaderModulesSSLiealgebras
#define vpfHeader_HeaderModulesSSLiealgebras
#include "math_general.h"
#include "math_extra_semisimple_Lie_algebras.h"

template <class Coefficient>
class ModuleSSalgebra {
  List<MatrixTensor<Coefficient> > actionsGeneratorsMaT;
  List<List<List<ElementUniversalEnveloping<Coefficient> > > > actionsGeneratorS;
  Selection ComputedGeneratorActions;
  Rational hwtaabfSimpleGensOnly(
    const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& leftMon,
    const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& rightMon,
    ProgressReport* theProgressReport = nullptr
  );
  Rational hwTrace(
    const Pair<MonomialTensor<int, MathRoutines::IntUnsignIdentity>,
    MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& thePair,
    ProgressReport* theProgressReport = nullptr
  );
  void checkConsistency();
public:
  SemisimpleLieAlgebra* owner;
  HashedList<MonomialUniversalEnveloping<Coefficient> > theGeneratingWordsNonReduced;
  // Note: for some reason, the linker fails to resolve without the explicit template
  // specialization below.
  // [Update:] made a bug report on this in the gcc bug tracker.
  // This issue has officially been recognized as a gcc bug. Hope to get a fix soon.
  HashedListSpecialized<MonomialTensor<int, MathRoutines::IntUnsignIdentity> > theGeneratingWordsNonReducedInt;
  Vectors<Rational> theGeneratingWordsWeightsPlusWeightFDpart;
  List<LittelmannPath> thePaths;
  List<List<MonomialUniversalEnveloping<Coefficient> > > theGeneratingWordsGrouppedByWeight;
  List<List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> > > theGeneratingWordsIntGrouppedByWeight;
  // List<ElementUniversalEnveloping<Coefficient> > theSimpleGens;
  // List<List<List<ElementUniversalEnveloping<Coefficient> > > > actionsSimpleGens;
  // List<Matrix<Coefficient> > actionsSimpleGensMatrixForM;
  List<Matrix<Coefficient> > theBilinearFormsAtEachWeightLevel;
  List<Matrix<Coefficient> > theBilinearFormsInverted;
  // Vectors<Rational> weightsSimpleGens;
  Vector<Coefficient> theHWDualCoordsBaseFielD;
  Vector<Coefficient> theHWSimpleCoordSBaseField;
  Vector<Coefficient> theHWFundamentalCoordsBaseField;
  Vector<Rational> theHWFDpartDualCoords;
  Vector<Rational> theHWFDpartSimpleCoordS;
  Vector<Rational> theHWFDpartFundamentalCoordS;
  // List<List<Matrix<Coefficient> > >
  HashedList<Vector<Rational> > theModuleWeightsSimpleCoords;
  CharacterSemisimpleLieAlgebraModule<Coefficient> theCharOverH;
  CharacterSemisimpleLieAlgebraModule<Coefficient> theChaR;
  Selection parabolicSelectionNonSelectedAreElementsLevi;
  Selection parabolicSelectionSelectedAreElementsLevi;
  std::string highestWeightVectorNotation;
  // Note: for some reason, the linker fails to resolve without the
  // explicit template specialization below.
  // [Update:] This is now a recognized gcc bug.
  HashedListSpecialized<
    Pair<MonomialTensor<int, MathRoutines::IntUnsignIdentity>,
    MonomialTensor<int, MathRoutines::IntUnsignIdentity> >
  > cachedPairs;
  List<Rational> cachedTraces;
  bool flagIsInitialized;
  bool flagConjectureBholds;
  bool flagConjectureCholds;
  bool flagDeallocated;
  int NumCachedPairsBeforeSimpleGen;
  int NumRationalMultiplicationsAndAdditionsBeforeSimpleGen;
  int MaxNumCachedPairs;
  void reset();
  bool operator==(const ModuleSSalgebra<Coefficient>& other) {
    return
    this->owner == other.owner &&
    this->theHWFundamentalCoordsBaseField == other.theHWFundamentalCoordsBaseField &&
    this->parabolicSelectionNonSelectedAreElementsLevi == other.parabolicSelectionNonSelectedAreElementsLevi;
  }
  bool hasFreeAction(int generatorIndex) const {
    Vector<Rational> theWeight = this->getOwner().getWeightOfGenerator(generatorIndex);
    for (int i = 0; i < this->parabolicSelectionNonSelectedAreElementsLevi.cardinalitySelection; i ++) {
      if (theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[i]].isNegative()) {
        return true;
      }
    }
    return false;
  }
  bool hasZeroActionFDpart(int generatorIndex) const {
    Vector<Rational> theWeight = this->getOwner().getWeightOfGenerator(generatorIndex);
    for (int i = 0; i < this->parabolicSelectionNonSelectedAreElementsLevi.cardinalitySelection; i ++) {
      if (theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[i]].isPositive()) {
        return true;
      }
    }
    return false;
  }
  int getOffsetFromWeightIndex(int weightIndex) {
    int result = 0;
    for (int i = 0; i < weightIndex; i ++) {
      result += this->theGeneratingWordsGrouppedByWeight[i].size;
    }
    return result;
  }
  void applyTAA(MonomialTensor<int, MathRoutines::IntUnsignIdentity>& theMon);
  void getFDchar(CharacterSemisimpleLieAlgebraModule<Coefficient>& output);
  void substitution(const PolynomialSubstitution<Rational>& theSub);
//  List<ElementUniversalEnveloping<Coefficient> > theGeneratingWordsLittelmannForm;
//  HashedList<MonomialUniversalEnveloping<Coefficient> > theGeneratingMonsPBWform;
//  List
//  List<Matrix<Coefficient> > ActionsChevalleyGenerators;
  MatrixTensor<Coefficient>& getActionGeneratorIndex(int generatorIndex);
  MatrixTensor<Coefficient>& getActionSimpleGeneratorIndex(int generatorIndex);
  int minimalNumberOfVariables() {
    if (this->theHWFundamentalCoordsBaseField.size <= 0) {
      return - 1;
    }
    int result = 0;
    for (int i = 0; i < this->theHWFundamentalCoordsBaseField.size; i ++) {
      result = MathRoutines::maximum(
        result, this->theHWFundamentalCoordsBaseField[i].minimalNumberOfVariables()
      );
    }
    return result;
  }
  int getDimension() const {
    return this->theGeneratingWordsNonReduced.size;
  }
  bool checkInitialization() const {
    if (this->flagDeallocated) {
      global.fatal << "Use after free of ModuleSSalgebra. " << global.fatal;
    }
    if (this->owner == nullptr) {
      global.fatal << "ModuleSSalgebra does not have its owner Semisimple algebra properly set. " << global.fatal;
    }
    return true;
  }
  void intermediateStepForMakeFromHW(const Coefficient& theRingUnit, const Coefficient& theRingZero);
  bool makeFromHW(
    SemisimpleLieAlgebra& inputAlgebra,
    Vector<Coefficient>& HWFundCoords,
    const Selection& selNonSelectedAreElementsLevi,
    const Coefficient& theRingUnit,
    const Coefficient& theRingZero,
    std::string* outputReport,
    bool computeSimpleGens = true
  );
  SemisimpleLieAlgebra& getOwner() const {
    if (this->owner == nullptr) {
      global.fatal << "This is a programming error: calling getOwner() "
      << "on a non-initialized generalized Verma module. " << global.fatal;
    }
    return *this->owner;
  }
  void getAdActionHomogenousElt(
    ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
    Vector<Rational> & weightUEEltSimpleCoords,
    List<List<ElementUniversalEnveloping<Coefficient> > >& outputSortedByArgumentWeight,
    const Coefficient& theRingUnit,
    const Coefficient& theRingZero
  );
  void getMatrixHomogenousElt(
    ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
    List<List<ElementUniversalEnveloping<Coefficient> > >& outputSortedByArgumentWeight,
    Vector<Rational>& weightUEEltSimpleCoords,
    MatrixTensor<Coefficient>& output,
    const Coefficient& theRingUnit,
    const Coefficient& theRingZero
  );
  void expressAsLinearCombinationHomogenousElement(
    ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
    ElementUniversalEnveloping<Coefficient>& outputHomogeneous,
    int indexInputBasis,
    const Vector<Coefficient>& subHiGoesToIthElement,
    const Coefficient& theRingUnit,
    const Coefficient& theRingZero
  );
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string elementToStringHWV(FormatExpressions* theFormat = nullptr) const {
    if (this->highestWeightVectorNotation != "") {
      return this->highestWeightVectorNotation;
    }
    std::stringstream out;
    out << "v_{" << this->theHWFundamentalCoordsBaseField.toString(theFormat)
    << ", " << this->parabolicSelectionNonSelectedAreElementsLevi.toString() << "}";
    return out.str();
    //    return "hwv{}("+ this->getOwner().toStringLieAlgebraName(false) + "," + this->theHWFundamentalCoordsBaseField.toString(theFormat) + ","
    //    + Vector<Rational> (this->parabolicSelectionNonSelectedAreElementsLevi).toString(theFormat) + ")";
  }
  void splitOverLevi(
    std::string* report,
    Selection& splittingParSel,
    List<ElementUniversalEnveloping<Coefficient> >* outputEigenVectors = nullptr,
    Vectors<Coefficient>* outputWeightsFundCoords = nullptr,
    Vectors<Coefficient>* outputEigenSpace = nullptr,
    CharacterSemisimpleLieAlgebraModule<Coefficient>* outputChar = nullptr
  );
  void splitFDpartOverFKLeviRedSubalg(
    HomomorphismSemisimpleLieAlgebra& theHmm,
    Selection& LeviInSmall,
    List<ElementUniversalEnveloping<Coefficient> >* outputEigenVectors = nullptr,
    Vectors<Coefficient>* outputWeightsFundCoords = nullptr,
    Vectors<Coefficient>* outputEigenSpace = nullptr,
    std::stringstream* comments = nullptr
  );
  template<class ResultType>
  void getElementsNilradical(
    List<ElementUniversalEnveloping<ResultType> >& output,
    bool useNegativeNilradical,
    List<int>* outputListOfGenerators,
    bool useNilWeight,
    bool ascending
  );
  bool isNotInLevi(int theGeneratorIndex);
  bool isNotInParabolic(int theGeneratorIndex);
  void getGenericUnMinusElt(
    bool shiftPowersByNumVarsBaseField,
    ElementUniversalEnveloping<Polynomial<Rational> >& output,
    bool useNilWeight,
    bool ascending
  );
  void getGenericUnMinusElt(
    bool shiftPowersByNumVarsBaseField,
    ElementUniversalEnveloping<RationalFunction<Rational> >& output,
    bool useNilWeight,
    bool ascending
  );
  // The input of the following function is supposed to be an honest element of the
  // Universal enveloping,
  // i.e. inputElt is not allowed to have non-small integer exponents.
  bool getActionGeneralizedVermaModuleAsDifferentialOperator(
    ElementSemisimpleLieAlgebra<Rational>& inputElt,
    QuasiDifferentialOperator<Rational>& output,
    bool useNilWeight,
    bool ascending
  );
  bool getActionEulerOperatorPart(const MonomialP& theCoeff, ElementWeylAlgebra<Rational>& outputDO);
  ModuleSSalgebra() : owner(nullptr), flagIsInitialized(false), flagDeallocated(false), MaxNumCachedPairs(1000000) {
  }
  ~ModuleSSalgebra() {
    this->flagDeallocated = true;
  }
};

template<class Coefficient>
class ElementTensorsGeneralizedVermas :
public LinearCombination<MonomialTensorGeneralizedVermas<Coefficient>, Coefficient> {
public:
  bool multiplyOnTheLeft(
    const ElementUniversalEnveloping<Coefficient>& theUE,
    ElementTensorsGeneralizedVermas<Coefficient>& output,
    SemisimpleLieAlgebra& ownerAlgebra,
    const Coefficient& theRingUnit
  ) const;
  bool multiplyOnTheLeft(
    const MonomialUniversalEnveloping<Coefficient>& theUE,
    ElementTensorsGeneralizedVermas<Coefficient>& output,
    SemisimpleLieAlgebra& ownerAlgebra,
    const Coefficient& theRingUnit
  ) const;
  void tensorOnTheRight(
    const ElementTensorsGeneralizedVermas<Coefficient>& right
  );
  void multiplyByElementLieAlg(
    ElementTensorsGeneralizedVermas<Coefficient>& output,
    SemisimpleLieAlgebra& ownerAlgebra,
    int indexGenerator,
    const Coefficient& theRingUnit
  ) const;
  void multiplyBy(const ElementTensorsGeneralizedVermas<Coefficient>& standsOnTheRight);
  bool isHWV() const {
    if (this->coefficients.size != 1) {
      return false;
    }
    if (!this->coefficients[0].isEqualToOne()) {
      return false;
    }
    return (*this)[0].isHWV();
  }
  void makeHWV(ModuleSSalgebra<Coefficient>& theOwner, const Coefficient& theRingUnit);
  void substitution(const PolynomialSubstitution<Rational>& theSub, ListReferences<ModuleSSalgebra<Coefficient> >& theMods);
  void setNumberOfVariables(int GoalNumVars) {
    for (int i = 0; i < this->size; i ++) {
      this->theObjects[i].setNumberOfVariables(GoalNumVars);
    }
  }
  SemisimpleLieAlgebra& getOwnerSemisimple() const {
    return this->getOwnerModule().getOwner();
  }
  ModuleSSalgebra<Coefficient>& getOwnerModule() const {
    if (this->size() <= 0) {
      global.fatal << "This is a programming error: "
      << "calling getOwnerModule() on a tensor element which has no monomials. "
      << "This is not allowed as the index of "
      << "the owner modules are stored in the monomials. " << global.fatal;
    }
    const MonomialTensorGeneralizedVermas<Coefficient>& theMon = (*this)[0];
    if (theMon.theMons.size <= 0) {
      global.fatal << "This is a programming error: calling getOwnerModule() "
      << "on a tensor element which has a constant monomial. "
      << "This is not allowed: constant monomials do not have owners. " << global.fatal;
    }
    MonomialGeneralizedVerma<Coefficient>& theGmon = theMon.theMons[0];
    return *theGmon.owner;
  }
  int minimalNumberOfVariables() {
    if (this->size == 0) {
      return - 1;
    }
    int theAnswer = this->theObjects[0].minimalNumberOfVariables();
    for (int i = 1; i < this->size; i ++) {
      if (theAnswer != this->theObjects[i].minimalNumberOfVariables()) {
        return - 1;
      }
    }
    return theAnswer;
  }
  unsigned int hashFunction() const {
    return this->::LinearCombination<MonomialTensorGeneralizedVermas<Coefficient>, Coefficient>::hashFunction();
  }
  ElementTensorsGeneralizedVermas<Coefficient>& operator=(const ElementTensorsGeneralizedVermas<Coefficient>& other) {
    this->::LinearCombination<MonomialTensorGeneralizedVermas<Coefficient>, Coefficient>::operator=(other);
    return *this;
  }
  ElementTensorsGeneralizedVermas<Coefficient>& operator=(const ElementSumGeneralizedVermas<Coefficient>& other);
  static unsigned int hashFunction(const ElementTensorsGeneralizedVermas<Coefficient>& input) {
    return input.hashFunction();
  }
};

#endif

