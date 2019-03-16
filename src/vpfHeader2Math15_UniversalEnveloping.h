//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_HeaderUniversalEnveloping
#define vpfHeader1_HeaderUniversalEnveloping
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math1_LieTheoryExtras.h"

static ProjectInformationInstance ProjectInfovpfHeader1_HeaderUniversalEnveloping(__FILE__, "Header, universal enveloping algebras. ");

template <class coefficient>
class MonomialUniversalEnveloping : public MonomialTensor<coefficient>
{
private:
public:
  std::string ToString(FormatExpressions* theFormat = 0) const;
  SemisimpleLieAlgebra* owner;
  // SelectedIndices gives the non-zero powers of the chevalley generators participating in the monomial
  // Powers gives the powers of the Chevalley generators in the order they appear in generatorsIndices
  friend std::ostream& operator << (std::ostream& output, const MonomialUniversalEnveloping<coefficient>& theMon) {
    output << theMon.ToString();
    return output;
  }
  bool IsConstant() const {
    return this->IsEqualToOne();
  }
  static bool IsMonEqualToZero() {
    return false;
  }
  bool AdjointRepresentationAction(const ElementUniversalEnveloping<coefficient>& input, ElementUniversalEnveloping<coefficient>& output) const;
  template<class otherType>
  void operator=(const MonomialUniversalEnveloping<otherType>& other) {
    this->MonomialTensor<coefficient>::operator=(other);
    this->owners = other.owners;
    this->indexInOwners = other.indexInOwners;
  }
  SemisimpleLieAlgebra& GetOwner() const {
    return *this->owner;
  }
  void MakeGenerator(int generatorIndex, SemisimpleLieAlgebra& inputOwner) {
    if (generatorIndex < 0 || generatorIndex>inputOwner.GetNumGenerators())
      crash << "This is a programming error: attempting to assign impossible index to monomial UE. " << crash;
    this->owner = &inputOwner;
    this->generatorsIndices.SetSize(1);
    this->generatorsIndices[0] = generatorIndex;
    this->Powers.SetSize(1);
    this->Powers[0] = 1;
  }
  bool HWTAAbilinearForm(
    const MonomialUniversalEnveloping<coefficient>& right,
    coefficient& output,
    const Vector<coefficient>* subHiGoesToIthElement,
    const coefficient& theRingUnit,
    const coefficient& theRingZero,
    std::stringstream* logStream = 0
  );
  void ModOutVermaRelations(
    coefficient& outputCoeff,
    const Vector<coefficient>* subHiGoesToIthElement = 0,
    const coefficient& theRingUnit = 1,
    const coefficient& theRingZero = 0
  );
  void SetNumVariables(int newNumVars);
  void Substitution(const PolynomialSubstitution<Rational>& theSub);
  unsigned int HashFunction() const {
    return this->::MonomialTensor<coefficient>::HashFunction();
  }
  static inline unsigned int HashFunction(const MonomialUniversalEnveloping<coefficient>& input) {
    return input.HashFunction();
  }
  void GetDegree(coefficient& output) const {
    if (this->Powers.size == 0) {
      output.MakeZero();
      return;
    }
    output = this->Powers[0];
    for (int i = 1; i < this->generatorsIndices.size; i ++)
      output += (this->Powers[i]);
  }
  bool CommutingABntoBnAPlusLowerOrderAllowed(coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex);
  bool CommutingAnBtoBAnPlusLowerOrderAllowed(coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex);
  bool SwitchConsecutiveIndicesIfTheyCommute(int theLeftIndex);
  void MakeOne(SemisimpleLieAlgebra& inputOwner) {
    this->generatorsIndices.size = 0;
    this->Powers.size = 0;
    this->owner = &inputOwner;
  }
  //we assume the standard order for being simplified to be Ascending.
  //this way the positive roots will end up being in the end, which is very
  //convenient for computing with Verma modules.
  //Format of the order of the generators:
  // first come the negative roots, in increasing height, then the elements of
  //the Cartan subalgebra, then the positive roots, in increasing height
  //The order of the positive roots is the same as the order in which roots are kept
  //in WeylGroup::RootSystem.
  // The "zero level roots" - i.e. the elements of the Cartan subalgebra lie in between
  // the negative and positive rootss.
  void Simplify(ElementUniversalEnveloping<coefficient>& output, const coefficient& theRingUnit = 1);
  void CommuteABntoBnAPlusLowerOrder(int theIndeX, ElementUniversalEnveloping<coefficient>& output, const coefficient& theRingUnit = 1);
  void CommuteAnBtoBAnPlusLowerOrder(int indexA, ElementUniversalEnveloping<coefficient>& output, const coefficient& theRingUnit = 1);
  MonomialUniversalEnveloping(): owner(0) {}
  bool operator>(const MonomialUniversalEnveloping& other) {
    return this->::MonomialTensor<coefficient>::operator>(other);
  }
  bool operator==(const MonomialUniversalEnveloping& other) const {
    return this->owner == other.owner && this->Powers == other.Powers && this->generatorsIndices == other.generatorsIndices;
  }
  inline void operator=(const MonomialUniversalEnveloping& other) {
    this->::MonomialTensor<coefficient>::operator=(other);
    this->owner = other.owner;
  }
  inline void operator*=(const MonomialUniversalEnveloping& other) {
    this->::MonomialTensor<coefficient>::operator*=(other);
  }
};

template <class coefficient>
class ElementUniversalEnveloping: public ElementMonomialAlgebra<MonomialUniversalEnveloping<coefficient>, coefficient> {
private:
  void CleanUpZeroCoeff();
  friend class MonomialUniversalEnveloping<coefficient>;
public:
  SemisimpleLieAlgebra* owner;
  bool AdjointRepresentationAction(const ElementUniversalEnveloping<coefficient>& input, ElementUniversalEnveloping<coefficient>& output) const;
  bool ConvertToRationalCoeff(ElementUniversalEnveloping<Rational>& output);
  bool IsEqualToZero() const {
    return this->size() == 0;
  }
  bool HWMTAbilinearForm
  (const ElementUniversalEnveloping<coefficient>&right, coefficient& output, const Vector<coefficient>* subHiGoesToIthElement,
   const coefficient& theRingUnit, const coefficient& theRingZero, std::stringstream* logStream = 0);
  std::string IsInProperSubmodule
  (const Vector<coefficient>* subHiGoesToIthElement, const coefficient& theRingUnit, const coefficient& theRingZero);
  bool HWTAAbilinearForm(
    const ElementUniversalEnveloping<coefficient>&right,
    coefficient& output,
    const Vector<coefficient>* subHiGoesToIthElement,
    const coefficient& theRingUnit,
    const coefficient& theRingZero,
    std::stringstream* logStream = 0
  ) const;
  bool HWTAAbilinearForm(
    const MonomialUniversalEnveloping<coefficient>& right,
    coefficient& output,
    const Vector<coefficient>* subHiGoesToIthElement,
    const coefficient& theRingUnit,
    const coefficient& theRingZero,
    std::stringstream* logStream = 0
  ) const {
    ElementUniversalEnveloping<coefficient> tempElt;
    tempElt.MakeZero(*this->owner);
    tempElt.AddMonomial(right, theRingUnit);
    return this->HWTAAbilinearForm(tempElt, output, subHiGoesToIthElement, theRingUnit, theRingZero, logStream);
  }
  bool NeedsBracketForMultiplication() {
    return this->size > 1;
  }
  bool ApplyMinusTransposeAutoOnMe();
  bool ApplyTransposeAntiAutoOnMe();
  void MakeHgenerator(const Vector<Rational>& input, SemisimpleLieAlgebra& inputOwner);
  void AssignElementLieAlgebra(const ElementSemisimpleLieAlgebra<Rational>& input, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit = 1);
  bool GetWithSimpleGeneratorsOnly(MonomialCollection<MonomialTensor<coefficient>, coefficient>& output);
  void MakeOneGenerator(int theIndex, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit);
  void MakeOneGeneratorCoeffOne(int theIndex, SemisimpleLieAlgebra& inputOwners, const coefficient& theRingUnit = 1);
  void MakeOneGeneratorCoeffOne(int theIndex, int numVars, SemisimpleLieAlgebra& inputOwner);
  void MakeOneGeneratorCoeffOne(const Vector<Rational>& rootSpace, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit = 1) {
    this->MakeOneGeneratorCoeffOne(inputOwner.GetGeneratorFromRoot(rootSpace), inputOwner, theRingUnit);
  }
  coefficient GetKillingFormProduct(const ElementUniversalEnveloping<coefficient>& right) const;
  void MakeZero(SemisimpleLieAlgebra& inputOwner);
  bool GetLieAlgebraElementIfPossible(ElementSemisimpleLieAlgebra<Rational>& output) const;
  void MakeConst(const Rational& coeff, int numVars, SemisimpleLieAlgebra& inputOwner);
  void MakeConst(const coefficient& coeff, SemisimpleLieAlgebra& inputOwner) {
    this->MakeZero(inputOwner);
    MonomialUniversalEnveloping<coefficient> tempMon;
    tempMon.MakeOne(inputOwner);
    this->AddMonomial(tempMon, coeff);
  }
  void Simplify(const coefficient& theRingUnit = 1);
  int GetMinNumVars() const {
    int result = 0;
    for (int i = 0; i < this->size; i ++) {
      result = MathRoutines::Maximum(result, this->theCoeffs[i].GetMinNumVars());
      result = MathRoutines::Maximum(result, (*this)[i].GetMinNumVars());
    }
    return result;
  }
  void MultiplyBy(const MonomialUniversalEnveloping<coefficient>& standsOnTheRight, const coefficient& theCoeff);
  void ModToMinDegreeFormFDRels(const Vector<Rational> & theHWinSimpleCoords, const coefficient& theRingUnit, const coefficient& theRingZero);
  void ModOutVermaRelations(const Vector<coefficient>* subHiGoesToIthElement, const coefficient& theRingUnit = 1, const coefficient& theRingZero = 0);
  static void GetCoordinateFormOfSpanOfElements(
    List<ElementUniversalEnveloping<coefficient> >& theElements,
    Vectors<coefficient>& outputCoordinates,
    ElementUniversalEnveloping<coefficient>& outputCorrespondingMonomials
  );
  bool GetCoordsInBasis(
    List<ElementUniversalEnveloping<coefficient> >& theBasis,
    Vector<coefficient>& output,
    const coefficient& theRingUnit,
    const coefficient& theRingZero
  ) const;
  static inline unsigned int HashFunction (const ElementUniversalEnveloping<coefficient>& input) {
    return input.HashFunction();
  }
  unsigned int HashFunction() const {
    return this->::MonomialCollection<MonomialUniversalEnveloping<coefficient>, coefficient>::HashFunction();
  }
  template<class CoefficientTypeQuotientField>
  static bool GetBasisFromSpanOfElements(
    List<ElementUniversalEnveloping<coefficient> >& theElements,
    Vectors<CoefficientTypeQuotientField>& outputCoords,
    List<ElementUniversalEnveloping<coefficient> >& outputTheBasis,
    const CoefficientTypeQuotientField& theFieldUnit,
    const CoefficientTypeQuotientField& theFieldZero
  );
  void AssignFromCoordinateFormWRTBasis(List<ElementUniversalEnveloping<coefficient> >& theBasis, Vector<coefficient>& input, SemisimpleLieAlgebra& owner);
  void RaiseToPower(int thePower);
  bool IsAPowerOfASingleGenerator() const {
    if (this->size() != 1)
      return false;
    if (!this->theCoeffs[0].IsEqualToOne())
      return false;
    return (*this)[0].generatorsIndices.size == 1;
  }
  void SubstitutionCoefficients(
    PolynomialSubstitution<Rational>& theSub, const coefficient& theRingUnit, const coefficient& theRingZero
  );
  void Substitution(const PolynomialSubstitution<Rational>& theSub);
  void MakeCasimir(SemisimpleLieAlgebra& theOwner);
  void MakeCasimirWRTLeviParabolic(SemisimpleLieAlgebra& theOwner, const Selection& theLeviRoots);
  static void LieBracket(const ElementUniversalEnveloping<coefficient>& left, const ElementUniversalEnveloping<coefficient>& right, ElementUniversalEnveloping<coefficient>& output) {
    left.LieBracketOnTheRight(right, output);
    output.Simplify();
  }
  void LieBracketOnTheRight(const ElementUniversalEnveloping<coefficient>& right, ElementUniversalEnveloping<coefficient>& output) const;
  void LieBracketOnTheLeft(const ElementSemisimpleLieAlgebra<Rational>& left);
  void AssignInt(int coeff, int numVars, SemisimpleLieAlgebra& theOwner) {
    Rational tempRat = coeff;
    this->MakeConst(tempRat, numVars, &theOwner);
  }
  SemisimpleLieAlgebra& GetOwner() const {
    return *this->owner;
  }
  template <class otherType>
  void Assign(const ElementUniversalEnveloping<otherType>& other) {
    this->owners = other.owners;
    this->indexInOwners = other.indexInOwners;
    MonomialUniversalEnveloping<coefficient> tempMon;
    coefficient theCoeff;
    for (int i = 0; i < other.size; i ++) {
      tempMon = other[i];
      theCoeff = other.theCoeffs[i];
      this->AddMonomial(tempMon, theCoeff);
    }
  }
  void operator=(const Rational& other) {
    this->MakeConst(other, 0, *this->owner);
  }
  void operator=(const ElementUniversalEnveloping<coefficient>& other) {
    this->::MonomialCollection<MonomialUniversalEnveloping<coefficient>, coefficient>::operator=(other);
    this->owner = other.owner;
  }
  void operator*=(const ElementUniversalEnveloping<coefficient>& standsOnTheRight);
  void operator*=(const coefficient& other) {
    this->::MonomialCollection<MonomialUniversalEnveloping<coefficient>, coefficient>
    ::operator*=(other);
  }
  template<class otherType>
  void operator/=(const otherType& other) {
    this->::MonomialCollection<MonomialUniversalEnveloping<coefficient>, coefficient>
    ::operator/=(other);
  }
  ElementUniversalEnveloping<coefficient>():owner(0) {
   
  }
  ElementUniversalEnveloping<coefficient>(const ElementUniversalEnveloping<coefficient>& other) {
    this->operator=(other);
  }
};

#endif
