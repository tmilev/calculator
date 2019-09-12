//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfHeaderLieTheoryMiscellaneous
#define vpfHeaderLieTheoryMiscellaneous

#include "math_extra_1_semisimple_Lie_algebras.h"
//#include "math_extra_10_universal_enveloping.h"
static ProjectInformationInstance ProjectInfoVpfHeaderLieTheoryMiscellaneous(
  __FILE__, "Header, Lie theory, miscellaneous. "
);

//the following data is isolated in a struct because it is
//way too large a lump to pass separately
struct branchingData {
  HomomorphismSemisimpleLieAlgebra theHmm;
  FormatExpressions theFormat;
  Vector<RationalFunctionOld> theWeightFundCoords;
  charSSAlgMod<RationalFunctionOld> theAmbientChar;
  charSSAlgMod<RationalFunctionOld> theSmallCharFDpart;
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
  Vectors<RationalFunctionOld> outputWeightsFundCoordS;
  Vectors<RationalFunctionOld> outputWeightsSimpleCoords;
  Vectors<RationalFunctionOld> g2Weights;
  Vectors<RationalFunctionOld> g2DualWeights;
  Vectors<RationalFunctionOld> leviEigenSpace;
  Vectors<Rational> generatorsSmallSub;
  HashedList<RationalFunctionOld> theCharacterDifferences;
  List<ElementUniversalEnveloping<RationalFunctionOld> > outputEigenWords;
  List<RationalFunctionOld> theChars;
  List<ElementSumGeneralizedVermas<RationalFunctionOld> > theEigenVectorS;
  List<ElementUniversalEnveloping<RationalFunctionOld> > theUEelts;
  List<Rational> additionalMultipliers;
  List<RationalFunctionOld> theShapovalovProducts;
  List<ElementSumGeneralizedVermas<RationalFunctionOld> > theEigenVectorsLevi;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms WeylFD;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms WeylFDSmallAsSubInLarge;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms WeylFDSmall;
  bool flagUseNilWeightGeneratorOrder;
  bool flagAscendingGeneratorOrder;
  std::string GetStringCasimirProjector(int theIndex, const Rational& additionalMultiple);
  template <class coefficient>
  Vector<coefficient> ProjectWeight(Vector<coefficient>& input);
  void resetOutputData();
  void initAssumingParSelAndHmmInittedPart1NoSubgroups();
  void initAssumingParSelAndHmmInittedPart2Subgroups();
  void initAssumingParSelAndHmmInitted() {
    MacroRegisterFunctionWithName("branchingData::initAssumingParSelAndHmmInitted");
    this->initAssumingParSelAndHmmInittedPart1NoSubgroups();
    this->initAssumingParSelAndHmmInittedPart2Subgroups();
  }
  branchingData();
};

class SemisimpleLieAlgebraOrdered {
public:
  SemisimpleLieAlgebra* theOwner;
  //the format of the order is arbitrary except for the following requirements:
  //-All elements of the order must be either 1) nilpotent or 2) elements of the Cartan
  //-Let the number of positive roots be N and the rank be K. Then the indices N,..., N+K- 1 must
  //   correspond to the elements of the Cartan.
  List<ElementSemisimpleLieAlgebra<Rational> > theOrder;
  //The order of chevalley generators is as follows. First come negative roots,
  //then elements of cartan, then positive Vectors<Rational>
  //The weights are in increasing order
  //The i^th column of the matrix gives the coordinates of the i^th Chevalley generator
  //in the current coordinates
  Matrix<Rational> ChevalleyGeneratorsInCurrentCoords;
  void AssignGeneratorCoeffOne(int theIndex, ElementSemisimpleLieAlgebra<Rational>& output) {
    output.operator=(this->theOrder[theIndex]);
  }
  SemisimpleLieAlgebraOrdered();
  bool CheckInitialization() const;
  int GetDisplayIndexFromGeneratorIndex(int GeneratorIndex);
  void GetLinearCombinationFrom(ElementSemisimpleLieAlgebra<Rational>& input, Vector<Rational>& theCoeffs);
  void init(List<ElementSemisimpleLieAlgebra<Rational> >& inputOrder, SemisimpleLieAlgebra& owner);
  void initDefaultOrder(SemisimpleLieAlgebra& owner);
};

template <class coefficient>
Vector<coefficient> branchingData::ProjectWeight(Vector<coefficient>& input) {
  Vector<coefficient> result;
  Vector<coefficient> fundCoordsSmaller;
  fundCoordsSmaller.MakeZero(this->theHmm.theDomain().GetRank());
  for (int j = 0; j < this->theHmm.theDomain().GetRank(); j ++) {
    fundCoordsSmaller[j] = this->theHmm.theRange().theWeyl.RootScalarCartanRoot(input, theHmm.ImagesCartanDomain[j]);
    fundCoordsSmaller[j] /= this->theHmm.theDomain().theWeyl.CartanSymmetric.elements[j][j] / 2;
  }
  result = this->theHmm.theDomain().theWeyl.GetSimpleCoordinatesFromFundamental(fundCoordsSmaller);
  return result;
}

class GeneralizedVermaModuleCharacters {
public:
  inline static const std::string GetXMLClassName() {
    return "GeneralizedVermaCharacters";
  }
  PauseThread thePauseControlleR;
  List<Matrix<Rational> > theLinearOperators;
  //the first k variables correspond to the Cartan of the smaller Lie algebra
  //the next l variables correspond to the Cartan of the larger Lie algebra
  //the last variable is the projectivization
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
//  List<QuasiPolynomial> theMultiplicitiesExtremaCandidates;
  int UpperLimitChambersForDebugPurposes;
  int numNonZeroMults;
  Selection ParabolicLeviPartRootSpacesZeroStandsForSelected;
  Selection ParabolicSelectionSmallerAlgebra;
  List<Rational> theCoeffs;
  Vectors<Rational> theTranslationS;
  Vectors<Rational> theTranslationsProjectedBasisChanged;
  PartFractions thePfs;
//  List<Cone> allParamSubChambersRepetitionsAllowedConeForm;
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
  std::string ComputeMultsLargerAlgebraHighestWeight(
    Vector<Rational>& highestWeightLargerAlgebraFundamentalCoords, Vector<Rational>& parabolicSel
  );
  std::string CheckMultiplicitiesVsOrbits();
  std::string ElementToStringMultiplicitiesReport();
  void IncrementComputation(Vector<Rational>& parabolicSel);
  std::string PrepareReport();
  GeneralizedVermaModuleCharacters();
  bool CheckInitialization() const;
  void ReadFromDefaultFile();
  void WriteToDefaultFile();
  void WriteToFile(std::fstream& output);
  bool ReadFromFile(std::fstream& input) {
    std::string tempS;
    int numReadWords;
    Vector<Rational>  parSel;
    parSel = this->ParabolicLeviPartRootSpacesZeroStandsForSelected;
    input >> tempS >> this->computationPhase;
    if (tempS != "ComputationPhase:") {
      crash << "Reading generalized characters from file failed. " << crash;
    }
    bool result = true;
    if (this->computationPhase != 0) {
      result = this->ReadFromFileNoComputationPhase(input);
    }
    XML::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, this->GetXMLClassName());
    return result;
  }
  bool ReadFromFileNoComputationPhase(std::fstream& input);
  std::string PrepareReportOneCone(FormatExpressions& theFormat, const Cone& theCone);
  void GetProjection(int indexOperator, const Vector<Rational>& input, Vector<Rational> & output);
  void SplitByMultiplicityFreeWall(Cone& theCone, ConeComplex& output);
  void InitTheMaxComputation();
  void ComputeQPsFromChamberComplex();
  void GetSubFromIndex(
    PolynomialSubstitution<Rational>& outputSub,
    Matrix<LargeInt>& outputMat,
    LargeIntUnsigned& ouputDen,
    int theIndex
  );
  void SortMultiplicities();
  void GetSubFromNonParamArray(
    Matrix<Rational>& output, Vector<Rational>& outputTranslation, Vectors<Rational>& NonParams, int numParams
  );
  void initFromHomomorphism(Vector<Rational>& theParabolicSel, HomomorphismSemisimpleLieAlgebra& input);
  void TransformToWeylProjectiveStep1();
  void TransformToWeylProjectiveStep2();
  void TransformToWeylProjective(int indexOperator, Vector<Rational>& startingNormal, Vector<Rational>& outputNormal);
};

template<class coefficient>
class ElementUniversalEnvelopingOrdered;

template<class coefficient>
class MonomialUniversalEnvelopingOrdered {
  void SimplifyAccumulateInOutputNoOutputInit(
    ElementUniversalEnvelopingOrdered<coefficient>& output,
    const coefficient& theRingUnit = 1,
    const coefficient& theRingZero = 0
  );
  MonomialUniversalEnvelopingOrdered(const MonomialUniversalEnvelopingOrdered& other);
public:
  SemisimpleLieAlgebraOrdered* owner;
  std::string DebugString;
  std::string ToString(bool useLatex, bool useCalculatorFormat, FormatExpressions* PolyFormatLocal) const;
  void ComputeDebugString() {
    FormatExpressions PolyFormatLocal;
    this->DebugString = this->ToString(false, true, &PolyFormatLocal);
  }
  // SelectedIndices gives the non-zero powers of the generators participating in the monomial
  // Powers gives the powers of the generators in the order specified in the owner
  List<int> generatorsIndices;
  List<coefficient> Powers;
  coefficient Coefficient;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void MultiplyBy(
    const MonomialUniversalEnveloping<coefficient>& other,
    ElementUniversalEnvelopingOrdered<coefficient>& output
  );
  void MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const coefficient& thePower);
  void MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower);
  void MultiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other);
  void MakeZero(int numVars, SemisimpleLieAlgebraOrdered& theOwner);
  void MakeZero(const coefficient& theRingZero, SemisimpleLieAlgebraOrdered& theOwner);
  bool ModOutFDRelationsExperimental(
    const Vector<Rational>& theHWsimpleCoords,
    const coefficient& theRingUnit = 1,
    const coefficient& theRingZero = 0
  );
  void ModOutVermaRelations(
    const List<coefficient>* subHiGoesToIthElement = 0,
    const coefficient& theRingUnit = 1,
    const coefficient& theRingZero = 0
  );
  void SetNumVariables(int newNumVars);
  unsigned int HashFunction() const;
  static inline unsigned int HashFunction(const MonomialUniversalEnvelopingOrdered<coefficient>& input) {
    return input.HashFunction();
  }
  void GetDegree(Polynomial<Rational>& output) {
    output.MakeZero(this->Coefficient.NumVars);
    for (int i = 0; i < this->generatorsIndices.size; i ++) {
      output += this->Powers[i];
    }
  }
  bool GetElementUniversalEnveloping(ElementUniversalEnveloping<coefficient>& output, SemisimpleLieAlgebraOrdered& owner);
  bool IsEqualToZero() const {
    return this->Coefficient.IsEqualToZero();
  }
  bool CommutingLeftIndexAroundRightIndexAllowed(
    coefficient& theLeftPower,
    int leftGeneratorIndex,
    coefficient& theRightPower,
    int rightGeneratorIndex
  );
  bool CommutingRightIndexAroundLeftIndexAllowed(
    coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex
  );
  bool SwitchConsecutiveIndicesIfTheyCommute(
    int theLeftIndex,
    MonomialUniversalEnvelopingOrdered<coefficient>& output,
    const coefficient& theRingZero = 0
  );
  void MakeConst(const coefficient& theConst, SemisimpleLieAlgebraOrdered& theOwner) {
    this->generatorsIndices.size = 0;
    this->Powers.size = 0;
    this->Coefficient = theConst;
    this->owner = &theOwner;
  }
  void Simplify(
    ElementUniversalEnvelopingOrdered<coefficient>& output,
    const coefficient& theRingUnit = 1,
    const coefficient& theRingZero = 0
  );
  void CommuteConsecutiveIndicesLeftIndexAroundRight(
    int theIndeX,
    ElementUniversalEnvelopingOrdered<coefficient>& output,
    const coefficient& theRingUnit = 1,
    const coefficient& theRingZero = 0
  );
  void CommuteConsecutiveIndicesRightIndexAroundLeft(
    int theIndeX,
    ElementUniversalEnvelopingOrdered<coefficient>& output,
    const coefficient& theRingUnit = 1,
    const coefficient& theRingZero = 0
  );
  MonomialUniversalEnvelopingOrdered() {
    this->owner = nullptr;
  }
  void SubstitutionCoefficients(PolynomialSubstitution<Rational>& theSub);
  bool operator==(const MonomialUniversalEnvelopingOrdered& other) const {
    if (this->owner != other.owner) {
      crash << "Attempt to compare universal enveloping algebra monomials with different owners. " << crash;
    }
    return this->Powers == other.Powers && this->generatorsIndices == other.generatorsIndices;
  }
  void operator*=(const MonomialUniversalEnvelopingOrdered& other);
  template<class OtherCoefficientType>
  void AssignChangeCoefficientType(const MonomialUniversalEnvelopingOrdered<OtherCoefficientType>& other) {
    this->Coefficient = other.Coefficient;
    this->generatorsIndices = other.generatorsIndices.size;
    this->Powers.SetSize(other.Powers.size);
    for (int i = 0; i < this->Powers.size; i ++) {
      this->Powers[i] = other.Powers[i];
    }
  }
  inline void operator=(const MonomialUniversalEnvelopingOrdered& other) {
    this->generatorsIndices = other.generatorsIndices;
    this->Powers = other.Powers;
    this->Coefficient = other.Coefficient;
    this->owner = other.owner;
  }
};

template <class coefficient>
bool MonomialUniversalEnvelopingOrdered<coefficient>::flagAnErrorHasOccurredTimeToPanic = false;

template <class coefficient>
class ElementUniversalEnvelopingOrdered : public HashedList<MonomialUniversalEnvelopingOrdered<coefficient> > {
private:
  void AddMonomialNoCleanUpZeroCoeff(const MonomialUniversalEnvelopingOrdered<coefficient>& input);
  void CleanUpZeroCoeff();
  friend class MonomialUniversalEnvelopingOrdered<coefficient>;
public:
  std::string DebugString;
  void ToString(std::string& output, bool useLatex, bool useCalculatorFormat, FormatExpressions& PolyFormatLocal) const;
  std::string ToString(bool useLatex, bool useCalculatorFormat, FormatExpressions& PolyFormatLocal) const {
    std::string tempS;
    this->ToString(tempS, useLatex, useCalculatorFormat, PolyFormatLocal);
    return tempS;
  }
  std::string ToString(FormatExpressions& PolyFormatLocal) const {
    std::string tempS;
    this->ToString(tempS, true, true, PolyFormatLocal);
    return tempS;
  }
  bool NeedsParenthesisForMultiplication() const {
    return this->size > 1;
  }
  void ComputeDebugString() {
    FormatExpressions tempFormat;
    this->DebugString = this->ToString(tempFormat);
  }
  SemisimpleLieAlgebraOrdered* owner;
  void AddMonomial(const MonomialUniversalEnvelopingOrdered<coefficient>& input);
  void MakeHgenerator(const Vector<Rational> & input, int numVars, SemisimpleLieAlgebraOrdered& theOwner);
  void AssignElementLieAlgebra(
    const ElementSemisimpleLieAlgebra<Rational>& input,
    const coefficient& theRingUnit,
    const coefficient& theRingZero,
    SemisimpleLieAlgebraOrdered& theOwner
  );
  void MakeOneGenerator(
    int theIndex, const coefficient& theCoeff, SemisimpleLieAlgebraOrdered& owner
  );
  void MakeZero(SemisimpleLieAlgebraOrdered& theOwner);
  bool AssignElementUniversalEnveloping(
    ElementUniversalEnveloping<coefficient>& input,
    SemisimpleLieAlgebraOrdered& owner,
    const coefficient& theRingUnit,
    const coefficient& theRingZero
  );
  bool AssignMonomialUniversalEnveloping(
    MonomialUniversalEnveloping<coefficient>& input,
    const coefficient& inputCoeff,
    SemisimpleLieAlgebraOrdered& owner,
    const coefficient& theRingUnit,
    const coefficient& theRingZero
  );
  bool ModOutFDRelationsExperimental(
    const Vector<Rational> & theHWsimpleCoords,
    const coefficient& theRingUnit = 1,
    const coefficient& theRingZero = 0
  );
  bool IsEqualToZero() const {
    return this->size == 0;
  }
  bool GetElementUniversalEnveloping(ElementUniversalEnveloping<coefficient>& output, SemisimpleLieAlgebra& inputOwner);
  bool GetLieAlgebraElementIfPossible(ElementSemisimpleLieAlgebra<Rational>& output) const;
  void SubstitutionCoefficients(PolynomialSubstitution<Rational>& theSub);
  void MakeConst(const coefficient& coeff, SemisimpleLieAlgebraOrdered& theOwner) {
    this->MakeZero(theOwner);
    MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
    tempMon.MakeConst(coeff, theOwner);
    this->AddMonomial(tempMon);
  }
  void Simplify(const coefficient& theRingUnit = 1,  const coefficient& theRingZero = 0);
  int GetNumVars() const {
    if (this->size == 0) {
      return 0;
    } else {
      return this->TheObjects[0].Coefficient.NumVars;
    }
  }
  inline void MultiplyBy(const ElementUniversalEnvelopingOrdered& other) {
    this->operator*=(other);
  }
  void ModOutVermaRelations(
    const List<coefficient>* subHiGoesToIthElement = 0,
    const coefficient& theRingUnit = 1,
    const coefficient& theRingZero = 0
  );
  void ModOutVermaRelationSOld(
    bool SubHighestWeightWithZeroes,
    const PolynomialSubstitution<Rational>& highestWeightSub,
    const coefficient& theRingUnit
  );
  template<class CoefficientTypeQuotientField>
  static void GetBasisFromSpanOfElements(
    List<ElementUniversalEnvelopingOrdered<coefficient> >& theElements,
    Vectors<CoefficientTypeQuotientField>& outputCoords,
    List<ElementUniversalEnvelopingOrdered<coefficient> >& outputTheBasis,
    const CoefficientTypeQuotientField& theFieldUnit,
    const CoefficientTypeQuotientField& theFieldZero
  );
  static void GetBasisFromSpanOfElements(
    List<ElementUniversalEnvelopingOrdered>& theElements,
    Vectors<Polynomial<coefficient> >& outputCoordinates,
    List<ElementUniversalEnvelopingOrdered>& outputTheBasis
  );
  bool GetCoordsInBasis(
    List<ElementUniversalEnvelopingOrdered<coefficient> >& theBasis,
    Vector<coefficient>& output,
    const coefficient& theRingUnit,
    const coefficient& theRingZero
  ) const;
  static void GetCoordinateFormOfSpanOfElements(
    int numVars, List<ElementUniversalEnvelopingOrdered>& theElements,
    Vectors<Polynomial<coefficient> >& outputCoordinates,
    ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials
  );
  void AssignFromCoordinateFormWRTBasis(
    List<ElementUniversalEnveloping<coefficient> >& theBasis,
    Vector<Polynomial<coefficient> >& input,
    SemisimpleLieAlgebraOrdered& owner
  );
  void RaiseToPower(int thePower, const coefficient& theRingUnit);
  bool IsAPowerOfASingleGenerator() {
    if (this->size != 1) {
      return false;
    }
    MonomialUniversalEnvelopingOrdered<coefficient>& tempMon = this->TheObjects[0];
    if (!tempMon.Coefficient.IsEqualToOne()) {
      return false;
    }
    if (tempMon.generatorsIndices.size != 1) {
      return false;
    }
    return true;
  }
  void MakeCasimir(SemisimpleLieAlgebraOrdered& theOwner, int numVars);
  void ActOnMe(const ElementSemisimpleLieAlgebra<Rational>& theElt, ElementUniversalEnvelopingOrdered& output);
  void LieBracketOnTheRight(const ElementUniversalEnvelopingOrdered& right, ElementUniversalEnvelopingOrdered& output);
  void LieBracketOnTheRight(
    const ElementSemisimpleLieAlgebra<Rational>& right, const coefficient& ringUnit, const coefficient& ringZero
  );
  void operator=(const ElementUniversalEnvelopingOrdered& other) {
    this->::HashedList<MonomialUniversalEnvelopingOrdered<coefficient> >::operator=(other);
    this->owner = other.owner;
  }
  template<class OtherCoefficientType>
  void AssignChangeCoefficientType (const ElementUniversalEnvelopingOrdered<OtherCoefficientType>& other) {
    this->MakeZero(*other.owner);
    MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
    this->Reserve(other.size);
    for (int i = 0; i < other.size; i ++) {
      tempMon.AssignChangeCoefficientType(other[i]);
      this->AddMonomial(tempMon);
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
  bool IsProportionalTo(
    const ElementUniversalEnvelopingOrdered<coefficient>& other,
    coefficient& outputTimesMeEqualsOther,
    const coefficient& theRingZero
  ) const;
  ElementUniversalEnvelopingOrdered(const ElementUniversalEnvelopingOrdered& other) {
    this->operator=(other);
  }
  void ClearDenominators(coefficient& outputWasMultipliedBy, const coefficient& theRingUnit) {
    outputWasMultipliedBy = theRingUnit;
    coefficient currentCoeff;
    for (int i = 0; i < this->size; i ++) {
      MonomialUniversalEnvelopingOrdered<coefficient>& currentMon = this->TheObjects[i];
      currentMon.Coefficient.ClearDenominators(currentCoeff);
      for (int j = 0; j < this->size; j ++) {
        if (j != i) {
          this->TheObjects[j].Coefficient *= currentCoeff;
        }
      }
      outputWasMultipliedBy *= currentCoeff;
    }
  }
};

template <class coefficient>
class ElementVermaModuleOrdered {
public:
  ElementUniversalEnvelopingOrdered<coefficient> theElT;
  std::string DebugString;
  PolynomialSubstitution<coefficient> theSubNthElementIsImageNthCoordSimpleBasis;
  void ComputeDebugString() {
    FormatExpressions tempFormat;
    this->DebugString = this->ToString(tempFormat);
  }
  std::string ToString(const FormatExpressions& theFormat) const;
  void ToString(std::string& output) const {
    output = this->ToString();
  }
  bool IsEqualToZero() const {
    return this->theElT.IsEqualToZero();
  }
  bool NeedsParenthesisForMultiplication() const;
  void AssignElementUniversalEnvelopingOrderedTimesHighestWeightVector(
    ElementUniversalEnvelopingOrdered<coefficient>& input,
    const ElementVermaModuleOrdered<coefficient>& theRingZero,
    const coefficient& theRingUnit
  );
  void ActOnMe(
    const ElementSemisimpleLieAlgebra<Rational>& actingElt,
    ElementVermaModuleOrdered<coefficient>& output,
    SemisimpleLieAlgebra& owner,
    const coefficient& theRingUnit,
    const coefficient& theRingZero
  ) const;
  static void GetBasisFromSpanOfElements(
    List<ElementVermaModuleOrdered<coefficient> >& theElements,
    Vectors<RationalFunctionOld>& outputCoordinates,
    List<ElementVermaModuleOrdered>& outputTheBasis,
    const RationalFunctionOld& RFOne,
    const RationalFunctionOld& RFZero
  );
  bool GetCoordsInBasis(
    const List<ElementVermaModuleOrdered<coefficient> >& theBasis,
    Vector<coefficient>& output,
    const coefficient& theRingUnit,
    const coefficient& theRingZero
  ) const;
  bool IsProportionalTo(
    const ElementVermaModuleOrdered<coefficient>& other,
    coefficient& outputTimesMeEqualsOther,
    const coefficient& theRingZero
  ) const {
    return this->theElT.IsProportionalTo(other.theElT, outputTimesMeEqualsOther, theRingZero);
  }
  void MakeZero(SemisimpleLieAlgebraOrdered& owner, PolynomialSubstitution<Rational>& incomingSub) {
    this->theElT.MakeZero(owner);
    this->theSubNthElementIsImageNthCoordSimpleBasis = incomingSub;
  }
  template <class CoefficientTypeOther>
  void operator*=(const CoefficientTypeOther& theConst) {
    this->theElT.operator*=(theConst);
  }
  void MultiplyOnTheLeft(
    const ElementSemisimpleLieAlgebra<Rational>& other,
    ElementVermaModuleOrdered<coefficient>& output,
    const coefficient& theRingUnit,
    const coefficient& theRingZero
  );
  void ClearDenominators(coefficient& outputWasMultipliedBy, const coefficient& theRingUnit) {
    this->theElT.ClearDenominators(outputWasMultipliedBy, theRingUnit);
  }
  void operator/=(const coefficient& theConst) {
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
