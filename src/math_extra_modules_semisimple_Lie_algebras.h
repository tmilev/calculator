// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader_HeaderModulesSSLiealgebras
#define vpfHeader_HeaderModulesSSLiealgebras
#include "math_general.h"
#include "math_extra_semisimple_Lie_algebras.h"

template <class coefficient>
class ModuleSSalgebra {
  List<MatrixTensor<coefficient> > actionsGeneratorsMaT;
  List<List<List<ElementUniversalEnveloping<coefficient> > > > actionsGeneratorS;
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
  void CheckConsistency();
public:
  SemisimpleLieAlgebra* owner;
  HashedList<MonomialUniversalEnveloping<coefficient> > theGeneratingWordsNonReduced;
  // Note: for some reason, the linker fails to resolve without the explicit template
  // specialization below.
  // [Update:] made a bug report on this in the gcc bug tracker.
  // This issue has officially been recognized as a gcc bug. Hope to get a fix soon.
  HashedListSpecialized<MonomialTensor<int, MathRoutines::IntUnsignIdentity> > theGeneratingWordsNonReducedInt;
  Vectors<Rational> theGeneratingWordsWeightsPlusWeightFDpart;
  List<LittelmannPath> thePaths;
  List<List<MonomialUniversalEnveloping<coefficient> > > theGeneratingWordsGrouppedByWeight;
  List<List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> > > theGeneratingWordsIntGrouppedByWeight;
  // List<ElementUniversalEnveloping<coefficient> > theSimpleGens;
  // List<List<List<ElementUniversalEnveloping<coefficient> > > > actionsSimpleGens;
  // List<Matrix<coefficient> > actionsSimpleGensMatrixForM;
  List<Matrix<coefficient> > theBilinearFormsAtEachWeightLevel;
  List<Matrix<coefficient> > theBilinearFormsInverted;
  // Vectors<Rational> weightsSimpleGens;
  Vector<coefficient> theHWDualCoordsBaseFielD;
  Vector<coefficient> theHWSimpleCoordSBaseField;
  Vector<coefficient> theHWFundamentalCoordsBaseField;
  Vector<Rational> theHWFDpartDualCoords;
  Vector<Rational> theHWFDpartSimpleCoordS;
  Vector<Rational> theHWFDpartFundamentalCoordS;
  // List<List<Matrix<coefficient> > >
  HashedList<Vector<Rational> > theModuleWeightsSimpleCoords;
  charSSAlgMod<coefficient> theCharOverH;
  charSSAlgMod<coefficient> theChaR;
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
  bool operator==(const ModuleSSalgebra<coefficient>& other) {
    return
    this->owner == other.owner &&
    this->theHWFundamentalCoordsBaseField == other.theHWFundamentalCoordsBaseField &&
    this->parabolicSelectionNonSelectedAreElementsLevi == other.parabolicSelectionNonSelectedAreElementsLevi;
  }
  bool HasFreeAction(int generatorIndex) const {
    Vector<Rational> theWeight = this->GetOwner().GetWeightOfGenerator(generatorIndex);
    for (int i = 0; i < this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; i ++) {
      if (theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[i]].IsNegative()) {
        return true;
      }
    }
    return false;
  }
  bool HasZeroActionFDpart(int generatorIndex) const {
    Vector<Rational> theWeight = this->GetOwner().GetWeightOfGenerator(generatorIndex);
    for (int i = 0; i < this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; i ++) {
      if (theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[i]].IsPositive()) {
        return true;
      }
    }
    return false;
  }
  int GetOffsetFromWeightIndex(int weightIndex) {
    int result = 0;
    for (int i = 0; i < weightIndex; i ++) {
      result += this->theGeneratingWordsGrouppedByWeight[i].size;
    }
    return result;
  }
  void ApplyTAA(MonomialTensor<int, MathRoutines::IntUnsignIdentity>& theMon);
  void GetFDchar(charSSAlgMod<coefficient>& output);
  void Substitution(const PolynomialSubstitution<Rational>& theSub);
//  List<ElementUniversalEnveloping<coefficient> > theGeneratingWordsLittelmannForm;
//  HashedList<MonomialUniversalEnveloping<coefficient> > theGeneratingMonsPBWform;
//  List
//  List<Matrix<coefficient> > ActionsChevalleyGenerators;
  MatrixTensor<coefficient>& GetActionGeneratorIndeX(int generatorIndex);
  MatrixTensor<coefficient>& GetActionSimpleGeneratorIndex(int generatorIndex);
  int minimalNumberOfVariables() {
    if (this->theHWFundamentalCoordsBaseField.size <= 0) {
      return - 1;
    }
    int result = 0;
    for (int i = 0; i < this->theHWFundamentalCoordsBaseField.size; i ++) {
      result = MathRoutines::Maximum(
        result, this->theHWFundamentalCoordsBaseField[i].minimalNumberOfVariables()
      );
    }
    return result;
  }
  int GetDim() const {
    return this->theGeneratingWordsNonReduced.size;
  }
  bool CheckInitialization() const {
    if (this->flagDeallocated) {
      global.fatal << "Use after free of ModuleSSalgebra. " << global.fatal;
    }
    if (this->owner == nullptr) {
      global.fatal << "ModuleSSalgebra does not have its owner Semisimple algebra properly set. " << global.fatal;
    }
    return true;
  }
  void IntermediateStepForMakeFromHW(const coefficient& theRingUnit, const coefficient& theRingZero);
  bool MakeFromHW(
    SemisimpleLieAlgebra& inputAlgebra,
    Vector<coefficient>& HWFundCoords,
    const Selection& selNonSelectedAreElementsLevi,
    const coefficient& theRingUnit,
    const coefficient& theRingZero,
    std::string* outputReport,
    bool computeSimpleGens = true
  );
  SemisimpleLieAlgebra& GetOwner() const {
    if (this->owner == nullptr) {
      global.fatal << "This is a programming error: calling GetOwner() "
      << "on a non-initialized generalized Verma module. " << global.fatal;
    }
    return *this->owner;
  }
  void GetAdActionHomogenousElT(
    ElementUniversalEnveloping<coefficient>& inputHomogeneous,
    Vector<Rational> & weightUEEltSimpleCoords,
    List<List<ElementUniversalEnveloping<coefficient> > >& outputSortedByArgumentWeight,
    const coefficient& theRingUnit,
    const coefficient& theRingZero
  );
  void GetMatrixHomogenousElt(
    ElementUniversalEnveloping<coefficient>& inputHomogeneous,
    List<List<ElementUniversalEnveloping<coefficient> > >& outputSortedByArgumentWeight,
    Vector<Rational>& weightUEEltSimpleCoords,
    MatrixTensor<coefficient>& output,
    const coefficient& theRingUnit,
    const coefficient& theRingZero
  );
  void ExpressAsLinearCombinationHomogenousElement(
    ElementUniversalEnveloping<coefficient>& inputHomogeneous,
    ElementUniversalEnveloping<coefficient>& outputHomogeneous,
    int indexInputBasis,
    const Vector<coefficient>& subHiGoesToIthElement,
    const coefficient& theRingUnit,
    const coefficient& theRingZero
  );
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string ElementToStringHWV(FormatExpressions* theFormat = nullptr) const {
    if (this->highestWeightVectorNotation != "") {
      return this->highestWeightVectorNotation;
    }
    std::stringstream out;
    out << "v_{" << this->theHWFundamentalCoordsBaseField.toString(theFormat)
    << ", " << this->parabolicSelectionNonSelectedAreElementsLevi.toString() << "}";
    return out.str();
    //    return "hwv{}("+ this->GetOwner().ToStringLieAlgebraName(false) + "," + this->theHWFundamentalCoordsBaseField.toString(theFormat) + ","
    //    + Vector<Rational> (this->parabolicSelectionNonSelectedAreElementsLevi).toString(theFormat) + ")";
  }
  void SplitOverLevi(
    std::string* Report,
    Selection& splittingParSel,
    List<ElementUniversalEnveloping<coefficient> >* outputEigenVectors = nullptr,
    Vectors<coefficient>* outputWeightsFundCoords = nullptr,
    Vectors<coefficient>* outputEigenSpace = nullptr,
    charSSAlgMod<coefficient>* outputChar = nullptr
  );
  void SplitFDpartOverFKLeviRedSubalg(
    HomomorphismSemisimpleLieAlgebra& theHmm,
    Selection& LeviInSmall,
    List<ElementUniversalEnveloping<coefficient> >* outputEigenVectors = nullptr,
    Vectors<coefficient>* outputWeightsFundCoords = nullptr,
    Vectors<coefficient>* outputEigenSpace = nullptr,
    std::stringstream* comments = nullptr
  );
  template<class ResultType>
  void GetElementsNilradical(
    List<ElementUniversalEnveloping<ResultType> >& output,
    bool useNegativeNilradical,
    List<int>* outputListOfGenerators,
    bool useNilWeight,
    bool ascending
  );
  bool IsNotInLevi(int theGeneratorIndex);
  bool IsNotInParabolic(int theGeneratorIndex);
  void GetGenericUnMinusElt(
    bool shiftPowersByNumVarsBaseField,
    ElementUniversalEnveloping<Polynomial<Rational> >& output,
    bool useNilWeight,
    bool ascending
  );
  void GetGenericUnMinusElt(
    bool shiftPowersByNumVarsBaseField,
    ElementUniversalEnveloping<RationalFunction>& output,
    bool useNilWeight,
    bool ascending
  );
  // The input of the following function is supposed to be an honest element of the
  // Universal enveloping,
  // i.e. inputElt is not allowed to have non-small integer exponents.
  bool GetActionGenVermaModuleAsDiffOperator(
    ElementSemisimpleLieAlgebra<Rational>& inputElt,
    quasiDiffOp<Rational>& output,
    bool useNilWeight,
    bool ascending
  );
  bool GetActionEulerOperatorPart(const MonomialP& theCoeff, ElementWeylAlgebra<Rational>& outputDO);
  ModuleSSalgebra() : owner(nullptr), flagIsInitialized(false), flagDeallocated(false), MaxNumCachedPairs(1000000) {
  }
  ~ModuleSSalgebra() {
    this->flagDeallocated = true;
  }
};

template<class coefficient>
class ElementTensorsGeneralizedVermas :
public LinearCombination<MonomialTensorGeneralizedVermas<coefficient>, coefficient> {
public:
  bool MultiplyOnTheLeft(
    const ElementUniversalEnveloping<coefficient>& theUE,
    ElementTensorsGeneralizedVermas<coefficient>& output,
    SemisimpleLieAlgebra& ownerAlgebra,
    const coefficient& theRingUnit
  ) const;
  bool MultiplyOnTheLeft(
    const MonomialUniversalEnveloping<coefficient>& theUE,
    ElementTensorsGeneralizedVermas<coefficient>& output,
    SemisimpleLieAlgebra& ownerAlgebra,
    const coefficient& theRingUnit
  ) const;
  void TensorOnTheRight(
    const ElementTensorsGeneralizedVermas<coefficient>& right
  );
  void MultiplyByElementLieAlg(
    ElementTensorsGeneralizedVermas<coefficient>& output,
    SemisimpleLieAlgebra& ownerAlgebra,
    int indexGenerator,
    const coefficient& theRingUnit
  ) const;
  void MultiplyBy(const ElementTensorsGeneralizedVermas<coefficient>& standsOnTheRight);
  bool IsHWV() const {
    if (this->coefficients.size != 1) {
      return false;
    }
    if (!this->coefficients[0].IsEqualToOne()) {
      return false;
    }
    return (*this)[0].IsHWV();
  }
  void MakeHWV(ModuleSSalgebra<coefficient>& theOwner, const coefficient& theRingUnit);
  void Substitution(const PolynomialSubstitution<Rational>& theSub, ListReferences<ModuleSSalgebra<coefficient> >& theMods);
  void SetNumVariables(int GoalNumVars) {
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i].SetNumVariables(GoalNumVars);
    }
  }
  SemisimpleLieAlgebra& GetOwnerSS() const {
    return this->GetOwnerModule().GetOwner();
  }
  ModuleSSalgebra<coefficient>& GetOwnerModule() const {
    if (this->size() <= 0) {
      global.fatal << "This is a programming error: "
      << "calling GetOwnerModule() on a tensor element which has no monomials. "
      << "This is not allowed as the index of "
      << "the owner modules are stored in the monomials. " << global.fatal;
    }
    const MonomialTensorGeneralizedVermas<coefficient>& theMon = (*this)[0];
    if (theMon.theMons.size <= 0) {
      global.fatal << "This is a programming error: calling GetOwnerModule() "
      << "on a tensor element which has a constant monomial. "
      << "This is not allowed: constant monomials do not have owners. " << global.fatal;
    }
    MonomialGeneralizedVerma<coefficient>& theGmon = theMon.theMons[0];
    return *theGmon.owner;
  }
  int GetNumVars() {
    if (this->size == 0) {
      return - 1;
    }
    int theAnswer = this->TheObjects[0].GetNumVars();
    for (int i = 1; i < this->size; i ++) {
      if (theAnswer != this->TheObjects[i].GetNumVars()) {
        return - 1;
      }
    }
    return theAnswer;
  }
  unsigned int HashFunction() const {
    return this->::LinearCombination<MonomialTensorGeneralizedVermas<coefficient>, coefficient >::HashFunction();
  }
  ElementTensorsGeneralizedVermas<coefficient>& operator=(const ElementTensorsGeneralizedVermas<coefficient>& other) {
    this->::LinearCombination<MonomialTensorGeneralizedVermas<coefficient>, coefficient>::operator=(other);
    return *this;
  }
  ElementTensorsGeneralizedVermas<coefficient>& operator=(const ElementSumGeneralizedVermas<coefficient>& other);
  static unsigned int HashFunction(const ElementTensorsGeneralizedVermas<coefficient>& input) {
    return input.HashFunction();
  }
};

#endif

