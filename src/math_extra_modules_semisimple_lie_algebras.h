// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader_HeaderModulesSSLiealgebras
#define vpfHeader_HeaderModulesSSLiealgebras
#include "math_general.h"
#include "math_extra_semisimple_lie_algebras.h"

template<class Coefficient>
class MonomialGeneralizedVerma {
  public:
  ModuleSSalgebra<Coefficient>* owner;
  MonomialUniversalEnveloping<Coefficient> monomialCoefficientOne;
  int indexFDVector;
  MonomialGeneralizedVerma(): owner(nullptr), indexFDVector(- 1) {
  }
  friend std::ostream& operator << (std::ostream& output, const MonomialGeneralizedVerma<Coefficient>& theGen) {
    output << theGen.toString();
    return output;
  }
  void multiplyMeByUEEltOnTheLeft(
    const ElementUniversalEnveloping<Coefficient>& theUE,
    ElementSumGeneralizedVermas<Coefficient>& output
  ) const;
  void operator=(const MonomialGeneralizedVerma<Coefficient>& other) {
    this->owner = other.owner;
    this->indexFDVector = other.indexFDVector;
    this->monomialCoefficientOne = other.monomialCoefficientOne;
  }

  std::string toString(FormatExpressions* format = nullptr, bool includeV = true) const;
  bool operator==(const MonomialGeneralizedVerma<Coefficient>& other) const {
    if (this->indexFDVector == other.indexFDVector && this->owner == other.owner) {
      return this->monomialCoefficientOne == other.monomialCoefficientOne;
    }
    return false;
  }
  void setNumberOfVariables(int goalNumVars) {
    if (this->owner->size <= this->indexInOwner) {
      global.fatal << "Crash in setNumberOfVariables: bad number of variables. " << global.fatal;
    }
    this->monomialCoefficientOne.setNumberOfVariables(goalNumVars);
    this->owner->objects[this->indexInOwner].setNumberOfVariables(goalNumVars);
  }
  void substitution(
    const PolynomialSubstitution<Rational>& theSub, ListReferences<ModuleSSalgebra<Coefficient> >& theMods
  );
  unsigned int hashFunction() const {
    return this->indexFDVector * someRandomPrimes[0] + (static_cast<unsigned int>(reinterpret_cast<uintptr_t>(this->owner))) * someRandomPrimes[1];
  }
  static unsigned int hashFunction(const MonomialGeneralizedVerma<Coefficient>& input) {
    return input.hashFunction();
  }
  bool operator>(const MonomialGeneralizedVerma<Coefficient>& other) const {
    if (this->owner != other.owner) {
      return reinterpret_cast<unsigned long>(this->owner) > reinterpret_cast<unsigned long>(other.owner);
    }
    if (this->indexFDVector != other.indexFDVector) {
      return this->indexFDVector > other.indexFDVector;
    }
    return this->monomialCoefficientOne > other.monomialCoefficientOne;
  }
  void reduceMe(ElementSumGeneralizedVermas<Coefficient>& output) const;
  bool isHWV() const {
    if (!this->monomialCoefficientOne.isEqualToOne()) {
      return false;
    }
    return this->getOwner().getDimension() - 1 == this->indexFDVector;
  }
  void makeConstant(ModuleSSalgebra<Coefficient>& inputOwner) {
    this->owner = &inputOwner;
    this->monomialCoefficientOne.makeOne(*inputOwner.owner);
  }
  ModuleSSalgebra<Coefficient>& getOwner() const {
    return *this->owner;
  }
};

template<class Coefficient>
class ElementSumGeneralizedVermas : public LinearCombination<MonomialGeneralizedVerma<Coefficient>, Coefficient> {
public:
  void multiplyMeByUEEltOnTheLeft(const ElementUniversalEnveloping<Coefficient>& theUE);
  unsigned int hashFunction() const {
    return this->LinearCombination<MonomialGeneralizedVerma<Coefficient>, Coefficient>::hashFunction();
  }
  static unsigned int hashFunction(const ElementSumGeneralizedVermas<Coefficient>& input) {
    return input.hashFunction();
  }
  ElementSumGeneralizedVermas() {
  }
  void makeHWV(ModuleSSalgebra<Coefficient>& theOwner, const Coefficient& ringUnit);
  int minimalNumberOfVariables() {
    if (this->owner == nullptr) {
      return - 1;
    }
    if (this->owner->size == 0) {
      return - 1;
    }
    int theAnswer = this->owner->objects[0].minimalNumberOfVariables();
    for (int i = 1; i < this->owner->size; i ++) {
      if (theAnswer != this->owner->objects[i].minimalNumberOfVariables()) {
        return - 1;
      }
    }
    return theAnswer;
  }
  bool extractElementUniversalEnveloping(ElementUniversalEnveloping<Coefficient>& output, SemisimpleLieAlgebra& theOwner);
  void operator=(const ElementSumGeneralizedVermas<Coefficient>& other) {
    this->::LinearCombination<MonomialGeneralizedVerma<Coefficient>, Coefficient>::operator=(other);
  }
};

template <class Coefficient>
class MonomialTensorGeneralizedVermas {
public:
  List<MonomialGeneralizedVerma<Coefficient> > monomials;
  friend std::ostream& operator<<(std::ostream& output, const MonomialTensorGeneralizedVermas<Coefficient>& input) {
    output << input.toString();
    return output;
  }
  void operator*=(const MonomialTensorGeneralizedVermas<Coefficient>& other) {
    if (this == &other) {
      MonomialTensorGeneralizedVermas<Coefficient> tempMon1;
      tempMon1 = other;
      *this *= tempMon1;
      return;
    }
    this->monomials.addListOnTop(other.monomials);
  }
  void operator*=(const MonomialGeneralizedVerma<Coefficient>& other) {
    this->monomials.addOnTop(other);
  }
  unsigned int hashFunction() const {
    int numCycles = MathRoutines::minimum(someRandomPrimesSize, this->monomials.size);
    unsigned int result = 0;
    for (int i = 0; i < numCycles; i ++) {
      result += someRandomPrimes[i] * this->monomials[i].hashFunction();
    }
    return result;
  }
  static unsigned int hashFunction(const MonomialTensorGeneralizedVermas<Coefficient>& input) {
    return input.hashFunction();
  }
  void setNumberOfVariables(int goalNumVars) {
    for (int i = 0; i < this->monomials.size; i ++) {
      this->monomials[i].setNumberOfVariables(goalNumVars);
    }
  }
  void substitution(
    const PolynomialSubstitution<Rational>& theSub,
    ListReferences<ModuleSSalgebra<Coefficient> >& theMods
  ) {
    for (int i = 0; i < this->monomials.size; i ++) {
      this->monomials[i].substitution(theSub, theMods);
    }
  }
  std::string toString(FormatExpressions* format = nullptr, bool includeV = true) const;
  MonomialTensorGeneralizedVermas() {
  }
  void operator=(const MonomialTensorGeneralizedVermas<Coefficient>& other) {
    this->monomials = other.monomials;
  }
  void operator=(const MonomialGeneralizedVerma<Coefficient>& other);
  bool operator==(const MonomialTensorGeneralizedVermas<Coefficient>& other) const {
    if (this->monomials.size != other.monomials.size) {
      return false;
    }
    for (int i = 0; i < this->monomials.size; i ++) {
      if (!(this->monomials[i] == other.monomials[i])) {
        return false;
      }
    }
    return true;
  }
  bool isHWV() const {
    if (this->monomials.size != 1) {
      return false;
    }
    return this->monomials[0].isHWV();
  }
  bool operator>(const MonomialTensorGeneralizedVermas<Coefficient>& other) const {
    if (this->monomials.size > other.monomials.size) {
      return true;
    }
    if (other.monomials.size > this->monomials.size) {
      return false;
    }
    ///This might need a rewrite. As it is, it will cause monomials to be sorted according to the
    ///alphabetical order of their human-readable strings. If I have time, I will make a better scheme for
    ///comparison.
    return this->toString() > other.toString();
  }
};

template <class Coefficient>
class ModuleSSalgebra {
  List<MatrixTensor<Coefficient> > actionsGeneratorsMaT;
  List<List<List<ElementUniversalEnveloping<Coefficient> > > > actionsGeneratorS;
  Selection ComputedGeneratorActions;
  Rational highestWeightTransposeAntiAutomorphismBilinearFormSimpleGeneratorsOnly(
    const MonomialTensor<int, HashFunctions::hashFunction>& leftMon,
    const MonomialTensor<int, HashFunctions::hashFunction>& rightMon,
    ProgressReport* theProgressReport = nullptr
  );
  Rational highestWeightTrace(
    const Pair<MonomialTensor<int, HashFunctions::hashFunction>,
    MonomialTensor<int, HashFunctions::hashFunction> >& thePair,
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
  HashedListSpecialized<MonomialTensor<int, HashFunctions::hashFunction> > theGeneratingWordsNonReducedInt;
  Vectors<Rational> theGeneratingWordsWeightsPlusWeightFDpart;
  List<LittelmannPath> thePaths;
  List<List<MonomialUniversalEnveloping<Coefficient> > > theGeneratingWordsGrouppedByWeight;
  List<List<MonomialTensor<int, HashFunctions::hashFunction> > > theGeneratingWordsIntGrouppedByWeight;
  // List<ElementUniversalEnveloping<Coefficient> > theSimpleGens;
  // List<List<List<ElementUniversalEnveloping<Coefficient> > > > actionsSimpleGens;
  // List<Matrix<Coefficient> > actionsSimpleGensMatrixForM;
  List<Matrix<Coefficient> > bilinearFormsAtEachWeightLevel;
  List<Matrix<Coefficient> > bilinearFormsInverted;
  // Vectors<Rational> weightsSimpleGens;
  Vector<Coefficient> theHWDualCoordsBaseFielD;
  Vector<Coefficient> theHWSimpleCoordSBaseField;
  Vector<Coefficient> theHWFundamentalCoordsBaseField;
  Vector<Rational> highestWeightFiniteDimensionalPartDualCoordinates;
  Vector<Rational> highestWeightFiniteDimensionalPartSimpleCoordinates;
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
    Pair<MonomialTensor<int, HashFunctions::hashFunction>,
    MonomialTensor<int, HashFunctions::hashFunction> >
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
  void applyTransposeAntiAutomorphism(MonomialTensor<int, HashFunctions::hashFunction>& monomial);
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
  void intermediateStepForMakeFromHW(const Coefficient& ringUnit, const Coefficient& ringZero);
  bool makeFromHW(
    SemisimpleLieAlgebra& inputAlgebra,
    Vector<Coefficient>& HWFundCoords,
    const Selection& selNonSelectedAreElementsLevi,
    const Coefficient& ringUnit,
    const Coefficient& ringZero,
    std::string* outputReport,
    bool computeSimpleGens = true
  );
  SemisimpleLieAlgebra& getOwner() const {
    if (this->owner == nullptr) {
      global.fatal << "Calling getOwner() "
      << "on a non-initialized generalized Verma module. " << global.fatal;
    }
    return *this->owner;
  }
  void getAdActionHomogenousElt(
    ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
    Vector<Rational> & weightUEEltSimpleCoords,
    List<List<ElementUniversalEnveloping<Coefficient> > >& outputSortedByArgumentWeight,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  void getMatrixHomogenousElt(
    ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
    List<List<ElementUniversalEnveloping<Coefficient> > >& outputSortedByArgumentWeight,
    Vector<Rational>& weightUEEltSimpleCoords,
    MatrixTensor<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  void expressAsLinearCombinationHomogenousElement(
    ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
    ElementUniversalEnveloping<Coefficient>& outputHomogeneous,
    int indexInputBasis,
    const Vector<Coefficient>& substitutionHiGoesToIthElement,
    const Coefficient& ringUnit,
    const Coefficient& ringZero
  );
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string elementToStringHWV(FormatExpressions* format = nullptr) const {
    if (this->highestWeightVectorNotation != "") {
      return this->highestWeightVectorNotation;
    }
    std::stringstream out;
    out << "v_{" << this->theHWFundamentalCoordsBaseField.toString(format)
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
  bool isNotInLevi(int generatorIndex);
  bool isNotInParabolic(int generatorIndex);
  void getGenericUnMinusElt(
    bool shiftPowersByNumVarsBaseField,
    ElementUniversalEnveloping<Polynomial<Rational> >& output,
    bool useNilWeight,
    bool ascending
  );
  void getGenericUnMinusElt(
    bool shiftPowersByNumVarsBaseField,
    ElementUniversalEnveloping<RationalFraction<Rational> >& output,
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
  bool getActionEulerOperatorPart(const MonomialPolynomial& coefficient, ElementWeylAlgebra<Rational>& outputDO);
  ModuleSSalgebra() : owner(nullptr), flagIsInitialized(false), flagDeallocated(false), MaxNumCachedPairs(1000000) {
  }
  ~ModuleSSalgebra() {
    this->flagDeallocated = true;
  }
  List<Matrix<Coefficient> > getTheBilinearFormsInverted() const;
  void setTheBilinearFormsInverted(const List<Matrix<Coefficient> >& value);
};

template<class Coefficient>
class ElementTensorsGeneralizedVermas :
public LinearCombination<MonomialTensorGeneralizedVermas<Coefficient>, Coefficient> {
public:
  bool multiplyOnTheLeft(
    const ElementUniversalEnveloping<Coefficient>& element,
    ElementTensorsGeneralizedVermas<Coefficient>& output,
    SemisimpleLieAlgebra& ownerAlgebra,
    const Coefficient& ringUnit
  ) const;
  bool multiplyOnTheLeft(
    const MonomialUniversalEnveloping<Coefficient>& theUE,
    ElementTensorsGeneralizedVermas<Coefficient>& output,
    SemisimpleLieAlgebra& ownerAlgebra,
    const Coefficient& ringUnit
  ) const;
  void tensorOnTheRight(
    const ElementTensorsGeneralizedVermas<Coefficient>& right
  );
  void multiplyByElementLieAlg(
    ElementTensorsGeneralizedVermas<Coefficient>& output,
    SemisimpleLieAlgebra& ownerAlgebra,
    int indexGenerator,
    const Coefficient& ringUnit
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
  void makeHWV(ModuleSSalgebra<Coefficient>& theOwner, const Coefficient& ringUnit);
  void substitution(const PolynomialSubstitution<Rational>& theSub, ListReferences<ModuleSSalgebra<Coefficient> >& theMods);
  void setNumberOfVariables(int goalNumVars) {
    for (int i = 0; i < this->size; i ++) {
      this->objects[i].setNumberOfVariables(goalNumVars);
    }
  }
  SemisimpleLieAlgebra& getOwnerSemisimple() const {
    return this->getOwnerModule().getOwner();
  }
  ModuleSSalgebra<Coefficient>& getOwnerModule() const {
    if (this->size() <= 0) {
      global.fatal
      << "Calling getOwnerModule() on a tensor element which has no monomials. "
      << "This is not allowed as the index of "
      << "the owner modules are stored in the monomials. " << global.fatal;
    }
    const MonomialTensorGeneralizedVermas<Coefficient>& monomial = (*this)[0];
    if (monomial.monomials.size <= 0) {
      global.fatal << "Calling getOwnerModule() "
      << "on a tensor element which has a constant monomial. "
      << "This is not allowed: constant monomials do not have owners. " << global.fatal;
    }
    MonomialGeneralizedVerma<Coefficient>& gMonomial = monomial.monomials[0];
    return *gMonomial.owner;
  }
  int minimalNumberOfVariables() {
    if (this->size == 0) {
      return - 1;
    }
    int answer = this->objects[0].minimalNumberOfVariables();
    for (int i = 1; i < this->size; i ++) {
      if (answer != this->objects[i].minimalNumberOfVariables()) {
        return - 1;
      }
    }
    return answer;
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
